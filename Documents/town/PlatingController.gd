class_name PlatingController

extends Panel

var is_plating : bool = true

# formatting --> {item_id: "smth_idk", topography: [left, right, height]}
var plating_content : Array[Dictionary] = []
var held_ingredient : String = "cheese" # item id

var temp_inventory : Dictionary = {
	"1": "bottom_bun",
	"2": "top_bun",
	"3": "cheese",
	"4": "patty"
}

@onready var plate : Control = $Plate
@onready var plate_width : float = $Plate.size.x
@onready var plate_height : float = $Plate.size.y
@onready var held_ingredient_ui : TextureRect = $Plate/HeldIngredient

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	change_ingredient(held_ingredient)
	
func x_position_plate(x : float) -> float:
	return x - plate.position.x
	
func change_ingredient(item_id : String) -> void:
	held_ingredient = item_id
	held_ingredient_ui.texture = GameManager.ITEM_INFO[item_id]["assembly_display"]["texture"]
	held_ingredient_ui.scale = Vector2.ONE * GameManager.ITEM_INFO[item_id]["assembly_display"]["scale"]
	
func actual_ingredient_width() -> float:
	return held_ingredient_ui.size.x * held_ingredient_ui.scale.x

func get_elevation(plate_x : float) -> float:
	var max_elevation = plate_height / 2 + 45
	for ingredient in plating_content:
		var left = ingredient["topography"][0]
		var right = ingredient["topography"][1]
		
		var within = left - actual_ingredient_width() / 2 <= plate_x
		within = within and plate_x <= right + actual_ingredient_width() / 2
		
		if within:
			max_elevation = min(max_elevation, ingredient["topography"][2])
	
	return max_elevation

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	visible = is_plating
	if is_plating:
		pass
		
func _input(event):
	if !is_plating:
		return
	
	if event is InputEventMouseMotion:
		var plate_x : float = x_position_plate(event.position.x)
		var centering_shift : float = actual_ingredient_width() / 2
		
		held_ingredient_ui.position.x = plate_x - centering_shift
		
		#if 0 <= plate_x and plate_x <= plate_width:
			#print("balls")
		#else:
			#print("cock")
	elif event is InputEventKey:
		if event.keycode == KEY_ESCAPE:
			# get rid of ingreidnt type shit
			pass
		if OS.get_keycode_string(event.keycode) in temp_inventory:
			var poopoo : String = temp_inventory[OS.get_keycode_string(event.keycode)]
			change_ingredient(poopoo)
	elif event is InputEventMouseButton and event.is_pressed() and event.button_index == MOUSE_BUTTON_LEFT:
		var plate_x : float = x_position_plate(event.position.x)
		var centering_shift : float = (held_ingredient_ui.size.x * held_ingredient_ui.scale.x) / 2
		
		held_ingredient_ui.position.x = plate_x - centering_shift
		
		if 0 <= plate_x and plate_x <= plate_width:
			print("ryhurguwgyuyguo")
			var new_ingredient = held_ingredient_ui.duplicate()
			$Plate.add_child(new_ingredient)
			
			var elevation : float = get_elevation(plate_x)
			new_ingredient.position.y = elevation - new_ingredient.size.y * new_ingredient.scale.y
			
			plating_content.append({
				"item_id": held_ingredient,
				"topography": [new_ingredient.position.x, 
					new_ingredient.position.x + new_ingredient.size.x * new_ingredient.scale.x,
					elevation - GameManager.ITEM_INFO[held_ingredient]["assembly_display"]["height"]]
			})
