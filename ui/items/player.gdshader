shader_type canvas_item;

uniform float tolerance: hint_range(0.0, 1.0, 0.01) = 1.0;
uniform vec4 prev_color: source_color = vec4(0.0, 1.0, 0.0, 1.0);
uniform vec4 new_color: source_color = vec4(1.0, 0.0, 1.0, 1.0);

void fragment() {
	if (new_color != prev_color)
	{
		vec4 color = texture(TEXTURE, UV);
		vec4 initial_color = color;
		float color_distance = distance(prev_color, color);
		if (color_distance <= tolerance) {
			color = mix(new_color, color, color_distance);
			color.a = initial_color.a;
		}
		COLOR = color;
	}
}
