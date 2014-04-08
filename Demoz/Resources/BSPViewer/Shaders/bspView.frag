#version 430

out vec4 color; //color fragment output
in vec2 gTexCoord; //uv texture coordinates (interpolated and clamped to [0.5,.95])
				  // hopefully, since the quad the texture is being mapped to is of the same size as the texture
				  // there will be a one-to-one correspondence between the sample coordinates of the texture and quad
uniform sampler2D tex; //the texture sampler

void main()
{
	color = texture(tex, gTexCoord*2);
	// color = vec4(1.0f, 1.0f, 0.0f, 1.0f)
}