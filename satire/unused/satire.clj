
(def STARTING-X 5)
(def STARTING-Y 5)

(defn find-radius [center-x center-y radius point-x point-y]
  (let [multi-x (- center-x point-x)
        multi-y (- center-y point-y)
        multi-xy (+ (* multi-x multi-x) (* multi-y multi-y))]
    (if (>= multi-xy (* radius radius))
      0
      1)))

(defn partial-find [center-x center-y radius]
  (loop [loop 0]
    (when (< loop radius)
      (let [radius-value (find-radius center-x center-y radius (- center-x loop) (- center-y loop))]
        (println "Some command")
        (println radius-value)
        (recur (+ loop 1 radius-value))))))

(partial-find STARTING-X STARTING-Y 5)
