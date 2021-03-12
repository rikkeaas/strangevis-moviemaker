#version 450

uniform mat4 modelViewProjectionMatrix;
uniform mat4 inverseModelViewProjectionMatrix;
uniform sampler3D volumeTexture;


in vec2 fragCoord;
out vec4 fragColor;

float calcDepth(vec3 pos)
{
	float far = gl_DepthRange.far; 
	float near = gl_DepthRange.near;
	vec4 clip_space_pos = modelViewProjectionMatrix * vec4(pos, 1.0);
	float ndc_depth = clip_space_pos.z / clip_space_pos.w;
	return (((far - near) * ndc_depth) + near + far) / 2.0;
}

void main() {
    vec4 near = modelViewProjectionMatrix * vec4(fragCoord, -1., 1.);
    near /= near.w;

    vec4 far = modelViewProjectionMatrix * vec4(fragCoord, 1., 1.);
    far /= far.w;

    vec3 rayOrigin = near.xyz;
    vec3 rayDir = normalize(far.xyz - near.xyz);

	float samplingDistance = 0.001;
	float renderDistance = 2.0;

	float firstValues = 0.0;
	bool notFound = true;

	vec3 sampligPoint = rayOrigin;
	for (float i = 0; i <= renderDistance; i += samplingDistance) 
	{
		float color = texture(volumeTexture, sampligPoint+1).r;
		if (color > 0.4) 
		{
			notFound = false;
			firstValues = color;
			break;
		}
		sampligPoint += rayDir * samplingDistance;
	}

	if (notFound)
	{
		fragColor = vec4(1.0);
		gl_FragDepth = 1.0;
	}
	else
	{
		fragColor = vec4(firstValues,firstValues,firstValues,1.0);
		gl_FragDepth = calcDepth(sampligPoint);
	}
}




















//in vec2 fragPosition;
//in vec4 near;
//in vec4 far;

/*
in vec4 fragPos;
out vec4 fragColor;


float calcDepth(vec3 pos)
{
	float far = gl_DepthRange.far; 
	float near = gl_DepthRange.near;
	vec4 clip_space_pos = modelViewProjectionMatrix * vec4(pos, 1.0);
	float ndc_depth = clip_space_pos.z / clip_space_pos.w;
	return (((far - near) * ndc_depth) + near + far) / 2.0;
}


void main()
{
	vec4 fragCoord = fragPos;
	fragCoord /= fragCoord.w;
	
	vec4 near = inverseModelViewProjectionMatrix*vec4(fragCoord.xy,-1.0,1.0);
	near /= near.w;

	vec4 far = inverseModelViewProjectionMatrix*vec4(fragCoord.xy,1.0,1.0);
	far /= far.w;

	vec3 dir = normalize(far.xyz-near.xyz);
	
	float ssamplingDistance = 0.001;
	float rrenderDistance = 2.0;

	float firstValues = 0.0;
	bool notFound = true;

	vec3 sampligPoint = fragCoord.xyz;

	for (float i = 0; i <= rrenderDistance; i += ssamplingDistance) 
	{
		float color = texture(volumeTexture, (normalize(near.xyz)+1)*0.5).r;// -1 -> 1
		if (color > 0.2) 
		{
			notFound = false;
			firstValues = color;
			break;
		}
		sampligPoint += dir * ssamplingDistance;
	}

	if (notFound)
	{
		fragColor = vec4(0.0);
		gl_FragDepth = 1.0;
	}
	else
	{
		fragColor = vec4(firstValues,firstValues,firstValues,1.0);
		gl_FragDepth = calcDepth(sampligPoint);
	}
/*
	vec4 fragCoord = fragPos;
	fragCoord /= fragCoord.w;
	
	vec4 near = inverseModelViewProjectionMatrix*vec4(fragCoord.xy,-1.0,1.0);
	near /= near.w;

	vec4 far = inverseModelViewProjectionMatrix*vec4(fragCoord.xy,1.0,1.0);
	far /= far.w;

	vec3 dir = normalize(far.xyz-near.xyz);
	
	float samplingDistance = 0.001;
	float renderDistance = 2.0;

	float firstValues = 0.0;
	bool notFound = true;

	for (float i = 0; i <= renderDistance; i += samplingDistance) 
	{
		float color = texture(volumeTexture, near.xyz).r;// -1 -> 1
		if (color > 0.2) 
		{
			notFound = false;
			firstValues = color;
			break;
		}
		near += dir * samplingDistance;
	}
	if (notFound)
	{
		fragColor = vec4(0.0);
		gl_FragDepth = 1.0;
	}
	else
	{
		fragColor = vec4(firstValues,firstValues,firstValues,1.0);
		gl_FragDepth = calcDepth(near.xyz);
	}

}

/*


struct Ray
{
	vec3 rayPos;
	float value;
};

Ray raycasting(vec3 origin, vec3 direction)
{
	int nbSteps = 0;
	float tex;
	bool foundSurface = false;
	while (nbSteps < 2000)
	{
		nbSteps += 1;
		origin += 0.005*direction;
		tex = texture(volumeTexture, (normalize(origin)+1.0)*0.5).r;
		if (tex >= 0.2)
		{
			foundSurface = true;
			break;
		}
	}
	if (!foundSurface) return Ray(origin, 0.0);
	return Ray(origin, tex);
}
*/

/*
void main() 
{
	vec3 origin = near.xyz/near.w;
    vec3 far3 = far.xyz/far.w;
    vec3 dir = far3 - origin;
    dir = normalize(dir);   
	
	float samplingDistance = 0.001;
	float renderDistance = 2.0;

	float firstValues = 0.0;
	bool notFound = true;

	for (float i = 0; i <= renderDistance; i += samplingDistance) {
		float color = texture(volumeTexture, vec3(normalize(modelViewProjectionMatrix*vec4(origin,1.0))+1)*0.5).r;// -1 -> 1
		if (color > 0.2) 
		{
			notFound = false;
			firstValues = color;
			break;
		}
		origin += dir * samplingDistance;
	}

	if (notFound)
	{
		fragColor = vec4(0.0);
		gl_FragDepth = 1.0;
	}
	else
	{
		fragColor = vec4(firstValues,firstValues,firstValues,1.0);
		gl_FragDepth = calcDepth(origin);
	}

}
*/
/*
void main()
{
	vec4 v = vec4(gl_FragCoord.xy,0.0,1.0);
	vec4 samplingPoint = vec4(fragPos.x, fragPos.y, 0.0, 1.0);
	float samplingDistance = 0.001;
	float renderDistance = 2.0;
	vec4 rayDirection = v - samplingPoint;
	
	vec3 volumeSize = textureSize(volumeTexture, 0);
	volumeSize = volumeSize / max(max(volumeSize.x, volumeSize.y), volumeSize.z);

	float firstValues = 0.0;
	bool notFound = true;
	for (float i = 0; i <= renderDistance; i += samplingDistance) {
		vec3 propPos = (vec3(samplingPoint) / volumeSize)/2.0+0.5;
		vec3 texPos = propPos.xyz;
		float color = texture(volumeTexture, texPos).r;// -1 -> 1
		if (color > 0.2) 
		{
			notFound = false;
			firstValues = color;
			break;
		}
		samplingPoint += rayDirection * samplingDistance;
	}

	if (notFound)
	{
		fragColor = vec4(0.0);
		gl_FragDepth = 1.0;
	}
	else
	{
		fragColor = vec4(firstValues,firstValues,firstValues,1.0);
		gl_FragDepth = calcDepth(samplingPoint.xyz);
	}
	/*
	vec4 near = inverseModelViewProjectionMatrix*vec4(fragPosition,-1.0,1.0);
	near /= near.w;

	vec4 far = inverseModelViewProjectionMatrix*vec4(fragPosition,1.0,1.0);
	far /= far.w;

	// this is the setup for our viewing ray
	vec3 rayOrigin = near.xyz;
	//vec3 rayDirection = normalize((far-near).xyz);

	Ray r = raycasting(rayOrigin, rayDirection);

	if (r.value == 0.0) 
	{
		fragColor=vec4(1.0);
		gl_FragDepth = 1.0;
	}
	else 
	{
		gl_FragDepth = calcDepth(r.rayPos);
		fragColor = vec4(r.value.rrr, 1.0);
	}
	
	//float tex = texture(volumeTexture, vec3((fragPosition+1.0)*0.5, 0.5)).r;
	//fragColor = vec4(tex.rrr, 1.0);

	// using calcDepth, you can convert a ray position to an OpenGL z-value, so that intersections/occlusions with the
	// model geometry are handled correctly, e.g.: gl_FragDepth = calcDepth(nearestHit);
	// in case there is no intersection, you should get gl_FragDepth to 1.0, i.e., the output of the shader will be ignored

	//gl_FragDepth = 0.5;
	
}

*/
/*
vec3 origin = near.xyz/near.w;
    vec3 far3 = far.xyz/far.w;
    vec3 dir = far3 - origin;
    dir = normalize(dir);   
	
	float samplingDistance = 0.001;
	float renderDistance = 2.0;

	float firstValues = 0.0;
	bool notFound = true;
*/