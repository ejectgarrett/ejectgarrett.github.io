class_name ClickableBody
extends PhysicsBody3D

signal clicked

@export var attached_body : MeshInstance3D
@export var _material_overlay : Material
var hovered : bool = false
var previously_hovered : bool = false

@onready var scene_manager : SceneManager = get_tree().current_scene

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if scene_manager.in_menu(): return
	
	if hovered != previously_hovered:
		if hovered:
			attached_body.material_overlay = _material_overlay
		else:
			attached_body.material_overlay = null
	previously_hovered = hovered
