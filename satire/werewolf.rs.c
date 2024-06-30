/****************************************************************

    werewolf.rs

    =============================================================

 Copyright 1996-2024 Tom Barbalet. All rights reserved.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    This software is a continuing work of Tom Barbalet, begun on
    13 June 1996. No apes or cats were harmed in the writing of
    this software.

****************************************************************/

use std::fs::File;
use std::io::{BufRead, BufReader, Write};

const LINELENGTH: usize = 200;

// This should be dynamic in the future. For now a fixed number
static mut GLOBALS: [u64; LINELENGTH] = [0; LINELENGTH];
static mut NUMBER_GLOBALS: usize = 0;

fn clear_globals() {
    unsafe {
        for global in GLOBALS.iter_mut() {
            *global = 0;
        }
        NUMBER_GLOBALS = 0;
    }
}

fn first_value(value: char) -> bool {
    value == '_' || value.is_ascii_alphabetic()
}

fn remaining_values(value: char) -> bool {
    value.is_ascii_alphanumeric() || first_value(value)
}

fn math_hash_fnv(key: &str) -> u64 {
    let mut hash = 2166136261_u64;
    let mut chars = key.chars();
    if let Some(first) = chars.next() {
        if first_value(first) {
            hash = ((8494653 * hash) ^ (first as u64)) as u64;
        } else {
            return 0;
        }
    }
    for ch in chars {
        if remaining_values(ch) {
            hash = ((8494653 * hash) ^ (ch as u64)) as u64;
        }
    }
    hash
}

fn global_found(key: &str) -> bool {
    let hash = math_hash_fnv(key);
    if hash == 0 {
        println!("no text found - investigate issue");
        return false;
    }
    unsafe {
        for &global in GLOBALS.iter() {
            if global == hash {
                return true;
            }
        }
    }
    false
}

fn add_global(key: &str) {
    let hash = math_hash_fnv(key);
    unsafe {
        GLOBALS[NUMBER_GLOBALS] = hash;
        NUMBER_GLOBALS += 1;
    }
}

fn clear_line_array(data: &mut [char]) {
    for item in data.iter_mut() {
        *item = '\0';
    }
}

fn copy_line_array(data_to: &mut [char], data_from: &[char]) {
    for (to, from) in data_to.iter_mut().zip(data_from.iter()) {
        *to = *from;
    }
}

fn copy_until_zero(in_buf: &mut [char], out: &[char]) -> usize {
    let mut loop_var = 0;
    while out[loop_var] != '\0' {
        in_buf[loop_var] = out[loop_var];
        loop_var += 1;
    }
    loop_var
}

fn combine_strings(in_location: usize, in_buf: &mut [char], add: &str) -> usize {
    let mut local_location = in_location;
    for ch in add.chars() {
        in_buf[local_location] = ch;
        local_location += 1;
    }
    local_location
}

fn parse_string_for_globals(in_buf: &str, out_buf: &mut String) -> bool {
    let mut something_has_changed = false;
    let mut location_in = 0;
    let mut location_internal = 0;
    let alpha_set = first_value(in_buf.chars().next().unwrap_or('\0'));
    let mut internal = ['\0'; LINELENGTH];
    let mut out_location = 0;

    clear_line_array(&mut internal);

    while let Some(ch) = in_buf.chars().nth(location_in) {
        if alpha_set != remaining_values(ch) {
            if alpha_set {
                if global_found(&internal.iter().collect::<String>()) {
                    out_buf.push('$');
                    something_has_changed = true;
                }
            }
            out_buf.push_str(&internal.iter().collect::<String>());

            location_internal = 0;
            clear_line_array(&mut internal);
        }
        internal[location_internal] = ch;
        location_in += 1;
        location_internal += 1;
    }
    if alpha_set {
        if global_found(&internal.iter().collect::<String>()) {
            out_buf.push('$');
            something_has_changed = true;
        }
    }
    out_buf.push_str(&internal.iter().collect::<String>());
    something_has_changed
}

fn line_compare(line: &str, check_for: &str) -> bool {
    line.starts_with(check_for)
}

fn contains_value(line: &str, contains: char) -> bool {
    line.contains(contains)
}

fn add_a_space(line: &mut String) {
    line.push(' ');
}

fn find_float(line: &str) -> bool {
    line.starts_with("float")
}

fn find_int(line: &str) -> bool {
    line.starts_with("int")
}

fn find_variable_number_array(line: &str, number: &mut String, array: &mut String, r#type: &mut String) {
    let mut loop_var = 4;
    if line.starts_with("float") {
        loop_var = 6;
    }
    if find_int(line) {
        *r#type = "int".to_string();
    }
    if find_float(line) {
        *r#type = "float".to_string();
    }

    if find_int(line) || find_float(line) {
        for (i, ch) in line[loop_var..].chars().enumerate() {
            if ch == '[' {
                array.push_str(&line[loop_var..loop_var + i]);
                loop_var += i + 1;
                break;
            }
        }
        for ch in line[loop_var..].chars() {
            if ch == ']' {
                break;
            }
            number.push(ch);
        }
    }
}

fn remove_replace(line: &str, new_line: &mut String, remove: &str, replace: Option<&str>) {
    let mut loop_var = 0;
    let mut remove_loop = 0;
    let mut replace_loop = 0;
    let mut reset_loop = -1;
    let mut reset_new_loop = -1;

    let mut temp_line = line.to_string();
    add_a_space(&mut temp_line);

    while let Some(value) = temp_line.chars().nth(loop_var) {
        let remove_part = remove.chars().nth(remove_loop);
        if remove_part.is_none() {
            if let Some(rep) = replace {
                replace_loop = 0;
                while let Some(ch) = rep.chars().nth(replace_loop) {
                    new_line.push(ch);
                    replace_loop += 1;
                }
            }
            reset_loop = -1;
            reset_new_loop = -1;
            remove_loop = 0;
        } else if remove_part.unwrap() != value {
            if reset_loop != -1 {
                loop_var = reset_loop;
            }
            reset_loop = -1;
            reset_new_loop = -1;
            remove_loop = 0;
            new_line.push(value);
            loop_var += 1;
        } else {
            if reset_loop == -1 {
                reset_loop = loop_var;
                reset_new_loop = new_line.len() as i32;
            }
            loop_var += 1;
            remove_loop += 1;
        }
    }
}

fn open_end_java(file: &mut File, open: bool, class_name: &str) {
    if open {
        writeln!(file, "public class {} {{", class_name).unwrap();
    } else {
        writeln!(file, "}}").unwrap();
    }
}

fn nothing_to_print_python(line: &str, new_line: &mut String, tabs: usize, no_print: bool) -> i32 {
    if no_print && !OUT_OF_MAIN.load(Ordering::SeqCst) {
        return 0;
    }

    if line_compare(line, "#") {
        return 0;
    }
    if line_compare(line, "int main") {
        OUT_OF_MAIN.store(false, Ordering::SeqCst);
        return 0;
    }

    remove_replace(line, new_line, ";", None);
    new_line.push_str(line);

    if line_compare(line, "return") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "printf(\"") {
        if no_print {
            return 0;
        }

        let mut temp_line = String::new();
        if contains_value(line, '%') {
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            remove_replace(line, &mut temp_line, "printf(", Some("print("));
            remove_replace(&temp_line, &mut temp_line2, "\"%d\\n\", ", None);
            remove_replace(&temp_line2, &mut temp_line3, "\"%f\\n\", ", None);
            remove_replace(&temp_line3, new_line, "\\n", None);
        } else {
            remove_replace(line, &mut temp_line, "printf(", Some("print("));
            remove_replace(&temp_line, new_line, "\\n", None);
        }
        return 2;
    }

    if line_compare(line, "//") {
        remove_replace(line, new_line, "// ", Some("#"));
        return 2;
    }
    if line_compare(line, "if") {
        remove_replace(line, new_line, ") {", Some("):"));
        return 2;
    }
    if line_compare(line, "}") {
        return 0;
    }
    if line_compare(line, "while") {
        remove_replace(line, new_line, ") {", Some("):"));
        return 2;
    }
    if line_compare(line, "const int ") {
        remove_replace(line, new_line, "const int ", None);
        return 2;
    }
    if line_compare(line, "const float ") {
        remove_replace(line, new_line, "const float ", None);
        return 2;
    }

    if line_compare(line, "int") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            let mut temp_line4 = String::new();

            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);

            remove_replace(line, &mut temp_line, "int ", Some("def "));
            remove_replace(&temp_line, &mut temp_line2, "float ", None);
            remove_replace(&temp_line2, &mut temp_line3, "int ", None);
            remove_replace(&temp_line3, &mut temp_line4, "void", None);
            remove_replace(&temp_line4, new_line, ") {", Some("):"));
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();

                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("{} = [0] * {}", array, number));
                return 2;
            } else {
                let mut temp_line = String::new();
                let mut temp_line2 = String::new();
                remove_replace(line, &mut temp_line, "int ", None);
                remove_replace(&temp_line, &mut temp_line2, "float ", None);
                remove_replace(&temp_line2, new_line, ") {", Some("):"));
                return 2;
            }
        }
        return 2;
    }
    if line_compare(line, "float") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            let mut temp_line4 = String::new();

            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);

            remove_replace(line, &mut temp_line, "float ", Some("def "));
            remove_replace(&temp_line, &mut temp_line2, "int ", None);
            remove_replace(&temp_line2, &mut temp_line3, "void", None);
            remove_replace(&temp_line3, &mut temp_line4, "float ", None);
            remove_replace(&temp_line4, new_line, ") {", Some("):"));
            return 2;
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();

                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("{} = [0] * {}", array, number));
                return 2;
            } else {
                let mut temp_line = String::new();
                let mut temp_line2 = String::new();
                remove_replace(line, &mut temp_line, "int ", None);
                remove_replace(&temp_line, &mut temp_line2, "float ", None);
                remove_replace(&temp_line2, new_line, ") {", Some("):"));
                return 2;
            }
        }
    }

    if line_compare(line, "void") {
        let mut temp_line = String::new();
        let mut temp_line2 = String::new();
        let mut temp_line3 = String::new();
        let mut temp_line4 = String::new();

        BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);

        remove_replace(line, &mut temp_line, "void ", Some("def "));
        remove_replace(&temp_line, &mut temp_line2, "int ", None);
        remove_replace(&temp_line2, &mut temp_line3, "float ", None);
        remove_replace(&temp_line3, &mut temp_line4, "void", None);
        remove_replace(&temp_line4, new_line, ") {", Some("):"));
    }
    2
}

fn nothing_to_print_ruby_post_process(line: &str, new_line: &mut String, tabs: usize, no_print: bool) -> i32 {
    clear_line_array(new_line);
    if parse_string_for_globals(line, new_line) {
        2
    } else {
        0
    }
}

fn nothing_to_print_ruby(line: &str, new_line: &mut String, tabs: usize, no_print: bool) -> i32 {
    if no_print && !OUT_OF_MAIN.load(Ordering::SeqCst) {
        return 0;
    }

    if line_compare(line, "#") {
        return 0;
    }
    if line_compare(line, "int main") {
        OUT_OF_MAIN.store(false, Ordering::SeqCst);
        return 0;
    }

    remove_replace(line, new_line, ";", None);
    new_line.push_str(line);

    if line_compare(line, "return") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "printf(\"") {
        if no_print {
            return 0;
        }

        let mut temp_line = String::new();
        let mut temp_line2 = String::new();

        if contains_value(line, '%') {
            let mut temp_line3 = String::new();
            let mut temp_line4 = String::new();
            remove_replace(line, &mut temp_line, "printf(", Some("puts "));
            remove_replace(&temp_line, &mut temp_line2, "\"%d\\n\", ", None);
            remove_replace(&temp_line2, &mut temp_line3, "\"%f\\n\", ", None);
            remove_replace(&temp_line3, &mut temp_line4, ")", None);
            remove_replace(&temp_line4, new_line, "\\n", None);
        } else {
            remove_replace(line, &mut temp_line, "printf(", Some("puts "));
            remove_replace(&temp_line, &mut temp_line2, ")", None);
            remove_replace(&temp_line2, new_line, "\\n", None);
        }
        return 2;
    }

    if line_compare(line, "//") {
        remove_replace(line, new_line, "// ", Some("#"));
        return 2;
    }
    if line_compare(line, "if") {
        remove_replace(line, new_line, "{", None);
        return 2;
    }
    if line_compare(line, "}") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            remove_replace(line, new_line, "}", Some("end"));
        } else {
            remove_replace(line, new_line, "}", None);
        }
        return 2;
    }
    if line_compare(line, "while") {
        remove_replace(line, new_line, "{", None);
        return 2;
    }
    if line_compare(line, "const int ") {
        remove_replace(line, new_line, "const int ", None);
        if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
            add_global(new_line);
        }
        return 2;
    }
    if line_compare(line, "const float ") {
        remove_replace(line, new_line, "const float ", None);
        if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
            add_global(new_line);
        }
        return 2;
    }

    if line_compare(line, "int") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            let mut temp_line4 = String::new();

            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);

            remove_replace(line, &mut temp_line, "int ", Some("def "));
            remove_replace(&temp_line, &mut temp_line2, "float ", None);
            remove_replace(&temp_line2, &mut temp_line3, "int ", None);
            remove_replace(&temp_line3, &mut temp_line4, "void", None);
            remove_replace(&temp_line4, new_line, ") {", Some(")"));
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();

                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("{} = Array.new({})", array, number));
                if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
                    add_global(&array);
                }
                return 2;
            } else {
                let mut temp_line = String::new();
                let mut temp_line2 = String::new();
                remove_replace(line, &mut temp_line, "int ", None);
                remove_replace(&temp_line, &mut temp_line2, "float ", None);
                remove_replace(&temp_line2, new_line, ") {", Some(")"));
                if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
                    add_global(&temp_line2);
                }
                return 2;
            }
        }
        return 2;
    }
    if line_compare(line, "float") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            let mut temp_line4 = String::new();

            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);

            remove_replace(line, &mut temp_line, "float ", Some("def "));
            remove_replace(&temp_line, &mut temp_line2, "int ", None);
            remove_replace(&temp_line2, &mut temp_line3, "void", None);
            remove_replace(&temp_line3, &mut temp_line4, "float ", None);
            remove_replace(&temp_line4, new_line, ") {", Some(")"));
            return 2;
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();

                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("{} = Array.new({})", array, number));
                if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
                    add_global(&array);
                }
                return 2;
            } else {
                let mut temp_line = String::new();
                let mut temp_line2 = String::new();
                remove_replace(line, &mut temp_line, "int ", None);
                remove_replace(&temp_line, &mut temp_line2, "float ", None);
                remove_replace(&temp_line2, new_line, ") {", Some(")"));
                if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
                    add_global(&temp_line2);
                }
                return 2;
            }
        }
    }

    if line_compare(line, "void") {
        let mut temp_line = String::new();
        let mut temp_line2 = String::new();
        let mut temp_line3 = String::new();
        let mut temp_line4 = String::new();

        BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);

        remove_replace(line, &mut temp_line, "void ", Some("def "));
        remove_replace(&temp_line, &mut temp_line2, "int ", None);
        remove_replace(&temp_line2, &mut temp_line3, "float ", None);
        remove_replace(&temp_line3, &mut temp_line4, "void", None);
        remove_replace(&temp_line4, new_line, ") {", Some(")"));
    }
    2
}

fn nothing_to_print_java(line: &str, new_line: &mut String, tabs: usize, no_print: bool) -> i32 {
    if no_print && !OUT_OF_MAIN.load(Ordering::SeqCst) {
        return 0;
    }

    if line_compare(line, "#") {
        return 0;
    }
    if line_compare(line, "int main") {
        OUT_OF_MAIN.store(false, Ordering::SeqCst);
        if no_print {
            return 0;
        }
        remove_replace(line, new_line, "int main (void) ", Some("public static void main(String[] args) "));
        return 2;
    }
    if line_compare(line, "return") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "printf(\"") {
        if no_print {
            return 0;
        }
        let mut temp_line = String::new();

        if contains_value(line, '%') {
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            remove_replace(line, &mut temp_line, "printf(", Some("System.out.println("));
            remove_replace(&temp_line, &mut temp_line2, "\"%d\\n\", ", None);
            remove_replace(&temp_line2, &mut temp_line3, "\"%f\\n\", ", None);
            remove_replace(&temp_line3, new_line, "\\n", None);
        } else {
            remove_replace(line, &mut temp_line, "printf(", Some("System.out.println("));
            remove_replace(&temp_line, new_line, "\\n", None);
        }
        return 2;
    }

    if line_compare(line, "//") {
        return 1;
    }
    if line_compare(line, "if") {
        return 1;
    }
    if line_compare(line, "}") {
        return 1;
    }
    if line_compare(line, "while") {
        return 1;
    }
    if line_compare(line, "const") {
        if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
            remove_replace(line, new_line, "const ", Some("public static final "));
        } else {
            remove_replace(line, new_line, "const ", Some("final "));
        }
        return 2;
    }
    if line_compare(line, "int") {
        if contains_value(line, '(') {
            let mut temp_line2 = String::new();
            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
            remove_replace(line, &mut temp_line2, "void", None);
            remove_replace(&temp_line2, new_line, "int ", Some("static int "));
            return 2;
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();
                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
                    new_line.push_str(&format!("public static {}[] {} = new {}[{}];", r#type, array, r#type, number));
                } else {
                    new_line.push_str(&format!("{}[] {} = new {}[{}];", r#type, array, r#type, number));
                }
                return 2;
            }
            return 1;
        }
    }
    if line_compare(line, "float") {
        if contains_value(line, '(') {
            let mut temp_line2 = String::new();
            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
            remove_replace(line, &mut temp_line2, "void", None);
            remove_replace(&temp_line2, new_line, "float ", Some("static float "));
            return 2;
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();
                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                if BEFORE_FUNCTIONS.load(Ordering::SeqCst) {
                    new_line.push_str(&format!("public static {}[] {} = new {}[{}];", r#type, array, r#type, number));
                } else {
                    new_line.push_str(&format!("{}[] {} = new {}[{}];", r#type, array, r#type, number));
                }
                return 2;
            }
            return 1;
        }
    }
    if line_compare(line, "void") {
        let mut temp_line2 = String::new();
        BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
        remove_replace(line, &mut temp_line2, "void", None);
        remove_replace(&temp_line2, new_line, "void ", Some("static void "));
        return 2;
    }
    1
}

fn nothing_to_print_javascript(line: &str, new_line: &mut String, tabs: usize, no_print: bool) -> i32 {
    if no_print && !OUT_OF_MAIN.load(Ordering::SeqCst) {
        return 0;
    }

    if line_compare(line, "#") {
        return 0;
    }
    if line_compare(line, "int main") {
        OUT_OF_MAIN.store(false, Ordering::SeqCst);
        return 0;
    }
    if line_compare(line, "return") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "printf(\"") {
        if no_print {
            return 0;
        }

        let mut temp_line = String::new();
        let mut temp_line2 = String::new();
        let mut temp_line3 = String::new();

        remove_replace(line, &mut temp_line, "printf(", Some("console.log("));
        remove_replace(&temp_line, &mut temp_line2, "\"%d\\n\", ", None);
        remove_replace(&temp_line2, &mut temp_line3, "\"%f\\n\", ", None);
        remove_replace(&temp_line3, new_line, "\\n", None);
        return 2;
    }

    if line_compare(line, "//") {
        return 1;
    }
    if line_compare(line, "if") {
        return 1;
    }
    if line_compare(line, "}") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "while") {
        return 1;
    }
    if line_compare(line, "const") {
        let mut temp_line = String::new();
        remove_replace(line, &mut temp_line, "float ", None);
        remove_replace(&temp_line, new_line, "int ", None);
        return 2;
    }
    if line_compare(line, "int") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
            remove_replace(line, &mut temp_line, "int ", Some("function "));
            remove_replace(&temp_line, &mut temp_line2, "void", None);
            remove_replace(&temp_line2, new_line, "int ", None);
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();
                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("var {} = [{}];", array, number));
            } else {
                new_line.push_str(&line.replace("int", "var"));
            }
        }
        return 2;
    }
    if line_compare(line, "float") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
            remove_replace(line, &mut temp_line, "float ", Some("function "));
            remove_replace(&temp_line, &mut temp_line2, "int ", None);
            remove_replace(&temp_line2, &mut temp_line3, "void", None);
            remove_replace(&temp_line3, new_line, "float ", None);
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();
                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("var {} = [{}];", array, number));
            } else {
                new_line.push_str(&line.replace("float", "var"));
            }
        }
        return 2;
    }
    if line_compare(line, "void") {
        let mut temp_line = String::new();
        let mut temp_line2 = String::new();
        let mut temp_line3 = String::new();
        BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
        remove_replace(line, &mut temp_line, "void ", Some("function "));
        remove_replace(&temp_line, &mut temp_line2, "int ", None);
        remove_replace(&temp_line2, &mut temp_line3, "void", None);
        remove_replace(&temp_line3, new_line, "float ", None);
        return 2;
    }
    1
}

fn nothing_to_print_rust(line: &str, new_line: &mut String, tabs: usize, no_print: bool) -> i32 {
    if no_print && !OUT_OF_MAIN.load(Ordering::SeqCst) {
        return 0;
    }

    if line_compare(line, "#") {
        return 0;
    }
    if line_compare(line, "int main") {
        OUT_OF_MAIN.store(false, Ordering::SeqCst);
        return 0;
    }
    if line_compare(line, "return") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "printf(\"") {
        if no_print {
            return 0;
        }

        let mut temp_line = String::new();
        let mut temp_line2 = String::new();
        let mut temp_line3 = String::new();

        remove_replace(line, &mut temp_line, "printf(", Some("println!("));
        remove_replace(&temp_line, &mut temp_line2, "\"%d\\n\", ", None);
        remove_replace(&temp_line2, &mut temp_line3, "\"%f\\n\", ", None);
        remove_replace(&temp_line3, new_line, "\\n", None);
        return 2;
    }

    if line_compare(line, "//") {
        return 1;
    }
    if line_compare(line, "if") {
        return 1;
    }
    if line_compare(line, "}") {
        if OUT_OF_MAIN.load(Ordering::SeqCst) {
            return 1;
        }
        return 0;
    }
    if line_compare(line, "while") {
        return 1;
    }
    if line_compare(line, "const") {
        let mut temp_line = String::new();
        remove_replace(line, &mut temp_line, "float ", None);
        remove_replace(&temp_line, new_line, "int ", None);
        return 2;
    }
    if line_compare(line, "int") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
            remove_replace(line, &mut temp_line, "int ", Some("fn "));
            remove_replace(&temp_line, &mut temp_line2, "void", None);
            remove_replace(&temp_line2, new_line, "int ", None);
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();
                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("let {} = [{}];", array, number));
            } else {
                new_line.push_str(&line.replace("int", "let"));
            }
        }
        return 2;
    }
    if line_compare(line, "float") {
        if contains_value(line, '(') {
            let mut temp_line = String::new();
            let mut temp_line2 = String::new();
            let mut temp_line3 = String::new();
            BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
            remove_replace(line, &mut temp_line, "float ", Some("fn "));
            remove_replace(&temp_line, &mut temp_line2, "int ", None);
            remove_replace(&temp_line2, &mut temp_line3, "void", None);
            remove_replace(&temp_line3, new_line, "float ", None);
        } else {
            if contains_value(line, '[') {
                let mut array = String::new();
                let mut number = String::new();
                let mut r#type = String::new();
                find_variable_number_array(line, &mut number, &mut array, &mut r#type);
                new_line.push_str(&format!("let {} = [{}];", array, number));
            } else {
                new_line.push_str(&line.replace("float", "let"));
            }
        }
        return 2;
    }
    if line_compare(line, "void") {
        let mut temp_line = String::new();
        let mut temp_line2 = String::new();
        let mut temp_line3 = String::new();
        BEFORE_FUNCTIONS.store(false, Ordering::SeqCst);
        remove_replace(line, &mut temp_line, "void ", Some("fn "));
        remove_replace(&temp_line, &mut temp_line2, "int ", None);
        remove_replace(&temp_line2, &mut temp_line3, "void", None);
        remove_replace(&temp_line3, new_line, "float ", None);
        return 2;
    }
    1
}

use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::atomic::AtomicBool;
static OUT_OF_MAIN: AtomicBool = AtomicBool::new(true);
static BEFORE_FUNCTIONS: AtomicBool = AtomicBool::new(true);

fn translate_file(filename: &str, writefilename: &str, no_print: bool, file_handler: fn(&str, &mut String, usize, bool) -> i32, post_process: Option<fn(&str, &mut String, usize, bool) -> i32>, class_name: Option<&str>, open_end: Option<fn(&mut File, bool, &str)>) {
    let tabs;
    let mut loop_var = 0;
    let mut line_array = ['\0'; LINELENGTH];
    let mut line_array_found = false;

    let reading = File::open(filename).unwrap();
    let mut writing = File::create(writefilename).unwrap();

    clear_line_array(&mut line_array);

    if class_name.is_some() {
        tabs = 1;
    } else {
        tabs = 0;
    }

    if let Some(open_end_func) = open_end {
        open_end_func(&mut writing, true, class_name.unwrap());
    }

    OUT_OF_MAIN.store(true, Ordering::SeqCst);
    BEFORE_FUNCTIONS.store(true, Ordering::SeqCst);

    let reader = BufReader::new(reading);
    for byte in reader.bytes() {
        let byte = byte.unwrap() as char;
        if byte == '\n' {
            line_array_found = true;
        } else {
            line_array[loop_var] = byte;
            loop_var += 1;
        }

        if line_array_found {
            let mut new_line = String::new();
            let value = file_handler(&line_array.iter().collect::<String>(), &mut new_line, tabs, no_print);

            if let Some(post_process_func) = post_process {
                let mut temp_line = new_line.clone();
                let new_value = post_process_func(&temp_line, &mut new_line, tabs, no_print);
                if value == 2 || new_value == 2 {
                    writing.write_all(new_line.as_bytes()).unwrap();
                }
            }

            if value != 0 {
                for _ in 0..tabs {
                    writing.write_all(b"\t").unwrap();
                }
            }

            if value == 2 {
                writing.write_all(new_line.as_bytes()).unwrap();
                writing.write_all(b"\n").unwrap();
            } else if value == 1 {
                writing.write_all(line_array.iter().collect::<String>().as_bytes()).unwrap();
                writing.write_all(b"\n").unwrap();
            }

            if class_name.is_some() {
                tabs = 1;
            } else {
                tabs = 0;
            }

            clear_line_array(&mut line_array);
            line_array_found = false;
            loop_var = 0;
        }
    }

    if let Some(open_end_func) = open_end {
        open_end_func(&mut writing, false, class_name.unwrap());
    }
}

fn parse_args(args: Vec<String>) -> (Option<String>, Option<String>, Option<String>, Option<String>, Option<String>, Option<String>, bool) {
    let mut csource = None;
    let mut python = None;
    let mut javascript = None;
    let mut java = None;
    let mut ruby = None;
    let mut rust = None;
    let mut no_print = false;

    let mut loop_var = 1;

    while loop_var < args.len() {
        let row = &args[loop_var];
        if row.starts_with('-') {
            match &row[1..] {
                "h" => {
                    println!("Usage: ./ww csourcefile [-js javascriptout | -j javaout | -p pythonout | -r rubyout]");
                }
                "v" => {
                    println!("v1.00.00");
                }
                "js" => {
                    loop_var += 1;
                    javascript = Some(args[loop_var].clone());
                }
                "j" => {
                    loop_var += 1;
                    java = Some(args[loop_var].clone());
                }
                "p" => {
                    loop_var += 1;
                    python = Some(args[loop_var].clone());
                }
                "r" => {
                    loop_var += 1;
                    ruby = Some(args[loop_var].clone());
                }
                "n" => {
                    no_print = true;
                }
                _ => {}
            }
        } else {
            csource = Some(args[loop_var].clone());
        }
        loop_var += 1;
    }
    (csource, python, javascript, java, ruby, rust, no_print)
}

fn get_class_name(java: &str) -> String {
    let mut class_name = String::new();
    for ch in java.chars() {
        if ch == '.' {
            break;
        }
        class_name.push(ch);
    }
    class_name
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let (csource, python, javascript, java, ruby, rust, no_print) = parse_args(args);

    if let Some(csource) = csource {
        if let Some(python) = python {
            println!("python : {}", python);
            println!("csource : {}", csource);
        }
        if let Some(javascript) = javascript {
            println!("javascript : {}", javascript);
        }
        if let Some(rust) = rust {
            println!("rust : {}", rust);
        }
        if let Some(ruby) = ruby {
            println!("ruby : {}", ruby);
        }
        if let Some(java) = java {
            println!("java : {}", java);
            let class_name = get_class_name(&java);
            println!("class name : {}", class_name);
        }

        if no_print {
            println!("No Print ON");
        }

        if let Some(rust) = rust {
            clear_globals();
            translate_file(&csource, &rust, no_print, nothing_to_print_rust, None, None, None);
        }
        if let Some(javascript) = javascript {
            translate_file(&csource, &javascript, no_print, nothing_to_print_javascript, None, None, None);
        }
        if let Some(python) = python {
            translate_file(&csource, &python, no_print, nothing_to_print_python, None, None, None);
        }
        if let Some(java) = java {
            let class_name = get_class_name(&java);
            translate_file(&csource, &java, no_print, nothing_to_print_java, None, Some(&class_name), Some(open_end_java));
        }
        if let Some(ruby) = ruby {
            clear_globals();
            translate_file(&csource, &ruby, no_print, nothing_to_print_ruby, Some(nothing_to_print_ruby_post_process), None, None);
        }
    }
}
