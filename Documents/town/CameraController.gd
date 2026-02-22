extends Camera3D

const SENSITIVITY : float = 0.4
@onready var body : CharacterBody3D = get_parent()

# Called when the node enters the scene tree for the first time.
func _ready():
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _unhandled_input(event):
	if event is InputEventMouseMotion:
		body.rotation_degrees.y -= event.relative.x * SENSITIVITY
		rotation_degrees.x -= event.relative.y * SENSITIVITY
		rotation_degrees.x = clamp(rotation_degrees.x, -70, 70)
	elif event is InputEventKey:
		if event.keycode == KEY_ESCAPE:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	elif event is InputEventMouseButton:
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
