class_name PlayerController
extends CharacterBody3D


const SPEED = 5.0
const JUMP_VELOCITY = 4.5

var inventory : Array[String] = ["empty", "empty", "empty"]

@onready var scene_manager : SceneManager = get_tree().current_scene

func _physics_process(delta: float) -> void:
	if scene_manager.in_menu(): return
	
	# Add the gravity.
	if not is_on_floor():
		velocity += get_gravity() * delta

	# Handle jump.
	#if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		#velocity.y = JUMP_VELOCITY

	# Get the input direction and handle the movement/deceleration.
	# As good practice, you should replace UI actions with custom gameplay actions.
	var input_dir := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	var direction := (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	if direction:
		velocity.x = direction.x * SPEED
		velocity.z = direction.z * SPEED
	else:
		velocity.x = move_toward(velocity.x, 0, SPEED)
		velocity.z = move_toward(velocity.z, 0, SPEED)

	move_and_slide()

func add_item(item_id: String) -> bool:
	if "empty" in inventory:
		var index : int = inventory.find("empty")
		inventory[index] = item_id
		scene_manager.update_inventory_slot(index, item_id)
		print(inventory)
		return true
	print("i had gay sex with a horse")
	return false
