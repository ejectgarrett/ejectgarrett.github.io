class_name SceneManager

extends Node3D

@onready var player_controller : PlayerController = $Player
@onready var camera : Camera3D = $Player/Camera3D

@onready var control : Control = $Control
@onready var dialog_ui : Control = $Control/Panel
@onready var inventory_slots : Array = $Control/Inventory.get_children()
@onready var plating_controller : PlatingController = $Control/Plating
#@onready var prompt_ui : Control = 

var hovered_collider : CollisionObject3D

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("sigma balls")
	pass # Replace with function body.

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta: float) -> void:
	var mouse_screen_pos = get_viewport().get_mouse_position()
	var ray_origin = camera.project_ray_origin(mouse_screen_pos)
	var ray_dir = camera.project_ray_normal(mouse_screen_pos)
	var ray_destination = ray_origin + ray_dir * 100
	
	var space_state = get_world_3d().direct_space_state
	var result = space_state.intersect_ray(PhysicsRayQueryParameters3D.create(ray_origin, ray_destination))
	
	if result:
		var new_collider : CollisionObject3D = result.collider
		if hovered_collider != new_collider:
			if hovered_collider is ClickableBody:
				hovered_collider.hovered = false
			if new_collider is ClickableBody:
				new_collider.hovered = true
			hovered_collider = new_collider
	
func _input(event):
	var clicked = event is InputEventMouseButton and event.is_pressed() and event.button_index == MOUSE_BUTTON_LEFT
	if !in_menu() and clicked and hovered_collider is ClickableBody:
		print("balls")
		hovered_collider.emit_signal("clicked")

func display_dialog(texture: Texture2D, name: String, contents: Array[String]) -> void:
	pass

func update_inventory_slot(slot: int, new_item_id: String) -> void:
	if new_item_id == "empty":
		inventory_slots[slot].get_child(0).text = ""
	else:
		inventory_slots[slot].get_child(0).text = new_item_id
		
func in_menu() -> bool:
	return plating_controller.is_plating
