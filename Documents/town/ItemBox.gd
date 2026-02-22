class_name ItemBox

extends MeshInstance3D

@export var capacity : int = 5
@export var count : int = 5
@export var item_id : String

@onready var scene_manager : SceneManager = get_tree().current_scene

func _ready() -> void:
	print("fuiprhoiuerhuierpigwhperiofj")

func restock() -> void:
	count = capacity

func take() -> bool:
	if count > 0:
		scene_manager.player_controller.add_item(item_id)
		return true
	return false
