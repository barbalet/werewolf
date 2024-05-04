# This is a contant
STARTING_X = 5
STARTING_Y = 5
TWO_PI = 6.0

$radiusValue = Array.new(5)

# This is a comment
def find_radius(center_x, center_y, radius, point_x, point_y)
  multi_x = center_x - point_x
  multi_y = center_y - point_y
  multi_xy = (multi_x * multi_x) + (multi_y * multi_y)
  if multi_xy >= (radius * radius)
    return 0
  else
    return 1
  end
end

def area_operator(find_x, find_y, loop)
  value_out = (TWO_PI * loop) / ((find_x * find_x) + (find_y * find_y))
  return value_out
end

def partial_find(center_x, center_y, radius)
  incrementer = 1
  loop = 0
  local_radius = Array.new(5)
  local_float = Array.new(5)
  while loop < radius
    find_x = center_x - loop
    find_y = center_y - loop
    $radiusValue[loop] = find_radius(center_x, center_y, radius, find_x, find_y)
    local_float[loop] = area_operator(find_x, find_y, loop)
    local_radius[loop] = $radiusValue[loop] + incrementer
    puts "Some command"
    puts $radiusValue[loop]
    puts local_float[loop]
    loop += local_radius[loop]
  end
end

partial_find(STARTING_X, STARTING_Y, 5)

