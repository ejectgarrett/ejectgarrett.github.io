extends Node3D

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	scale = lerp(scale, Vector3.ONE, 0.2)

func do_funny() -> void:
	scale *= 1.5
