#version 450

uniform mat4 modelViewProjectionMatrix;
uniform mat4 inverseModelViewProjectionMatrix;

uniform mat4 modelViewMatrix;
uniform mat4 inverseModelViewMatrix;

uniform mat4 rotate;
uniform mat4 invRotate;

uniform bool sphereCut;
uniform bool cubeCut;
uniform float cubeCutSize;
uniform float sphereCutRadius;
uniform bool showCut;
uniform bool showInFront;

uniform vec3 voxelSpacing;
uniform vec3 dimensionScaling;
uniform vec3 voxelDimsInTexCoord;

uniform vec3 lightPosition;

uniform sampler3D volumeTexture;
uniform sampler2D transferFunction;

uniform vec3 backgroundColorVector;

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




// Function to calculate the diffuse component of light at given position (assuming all light sources have the same diffuse intensity)
vec3 diffuseComponent(vec3 lightPos, vec3 pos, vec3 normal, vec3 color)
{
	vec3 pointToLight = normalize(lightPos - pos);
	vec3 diffuse = color * max(dot(normal,pointToLight),0.0f);
	return diffuse;
}

// Function to calculate the specular component of light at given position (assuming all light sources have the same specular intensity)
vec3 specularComponent(vec3 lightPos, vec3 pos, vec3 normal, vec3 color)
{
	vec3 camPos = (inverseModelViewProjectionMatrix * vec4(vec3(0.0), 1.0)).xyz;
	vec3 pointToEye = normalize(camPos - pos);
	vec3 lightToPoint = normalize(pos - lightPos);
	vec3 reflection = normalize(reflect(lightToPoint, normal));
	vec3 specular = color * pow(max(dot(reflection, pointToEye), 0.0f), 10.0);
	return specular;
}

// https://www.willusher.io/webgl/2019/01/13/volume-rendering-with-webgl
vec2 intersect_box(vec3 orig, vec3 dir) {
	vec3 inv_dir = 1.0 / dir;
	vec3 tmin_tmp = - orig * inv_dir - inv_dir * voxelSpacing * dimensionScaling;
	vec3 tmax_tmp = - orig * inv_dir + inv_dir * voxelSpacing * dimensionScaling;
	vec3 tmin = min(tmin_tmp, tmax_tmp);
	vec3 tmax = max(tmin_tmp, tmax_tmp);
	float t0 = max(tmin.x, max(tmin.y, tmin.z));
	float t1 = min(tmax.x, min(tmax.y, tmax.z));
	return vec2(t0, t1);
}


struct Intersection
{
	bool hit;
	vec3 enterPoint;
	vec3 exitPoint;
	vec3 normal;
};


Intersection calcBoxIntersection(vec3 box_min, vec3 box_max, vec3 origin, vec3 dir) {

	vec3 inv_dir = 1.0 / dir;
	vec3 tmin_tmp = (box_min - origin) * inv_dir;
	vec3 tmax_tmp = (box_max - origin) * inv_dir;
	vec3 tmin = min(tmin_tmp, tmax_tmp);
	vec3 tmax = max(tmin_tmp, tmax_tmp);
	float t0 = max(tmin.x, max(tmin.y, tmin.z));
	float t1 = min(tmax.x, min(tmax.y, tmax.z));
	if (t0 > t1)
	{
		return Intersection(false, vec3(0), vec3(0), vec3(0));
	}
	vec3 normDir = normalize(dir);
	return Intersection(true, ( vec4(origin + t0*dir, 1.0)).xyz, ( vec4(origin + t1*dir, 1.0)).xyz, vec3(1,0,0));
}

// Sphere-ray intersection test from my 251 project, sligthly modified
Intersection calcSphereIntersection(float r, vec3 center, vec3 origin, vec3 line)
{
	vec3 oc = origin - center;
	vec3 l = normalize(line);
	float ocProj = dot(oc, l); // projection of center of sphere onto ray
	float under_square_root = ocProj * ocProj - dot(oc,oc) + r*r;
	
	if (under_square_root > 0.0)
	{
		float da = -ocProj + sqrt(under_square_root);
		float ds = -ocProj - sqrt(under_square_root);

		if (da > 0 && ds > 0)
		{
			//vec4 near = invRotate * vec4(origin + min(da,ds) * l,1.0);
			//vec4 far = invRotate * vec4(origin + max(da,ds) * l, 1.0);
			vec4 near =vec4(origin + min(da,ds) * l,1.0);
			vec4 far = vec4(origin + max(da,ds) * l, 1.0);
			return Intersection(true, near.xyz, far.xyz, normalize(near.xyz - center));
		}
		if (max(da,ds) > 0)
		{
			//vec4 behindCam = invRotate * vec4(origin + min(da,ds) * l, 1.0);
			//vec4 intersect = invRotate * vec4(origin + max(da,ds) * l, 1.0);
			vec4 behindCam = vec4(origin + min(da,ds) * l, 1.0);
			vec4 intersect = vec4(origin + max(da,ds) * l, 1.0);
			return Intersection(true, behindCam.xyz, intersect.xyz, normalize(intersect.xyz - center));
		}
	}
	return Intersection(false, vec3(0), vec3(0), vec3(0));
}

/*
Intersection calcPlaneIntersection(vec3 planeOrigin, vec3 rayOrigin, vec3 rayDirection, vec3 samplePoint, vec3 planeNormal) 
{
	float denom = dot(planeNormal, rayDirection); 
    if (denom > 0) { 
        vec3 p0l0 = planeOrigin - rayOrigin; 
        float t = dot(p0l0, planeNormal) / denom; 
        if (t >= 0)
		{
			vec3 intP = rayOrigin + t * rayDirection;
			return Intersection(true, intP, intP, vec3(0));
		}
    } 
 
    return Intersection(false, vec3(0), vec3(0), vec3(0));
}
*/


vec3 scalePoint(vec3 point)
{
	return 0.5 + point / (voxelSpacing * dimensionScaling * 2.0);
}


void main() {
    vec4 near = inverseModelViewProjectionMatrix * vec4(fragCoord, -1.0, 1.0);
    near /= near.w;
    vec4 far = inverseModelViewProjectionMatrix * vec4(fragCoord, 1.0, 1.0);
    far /= far.w;

	vec3 rayOrigin = near.xyz;

    vec3 rayDir = normalize(far.xyz - near.xyz);

	vec2 t_hit = intersect_box(rayOrigin, rayDir);
	
	t_hit.x = max(t_hit.x, 0.0);


	float samplingDistance = 0.002;
	float gd = 0.001;
	float renderDistance = t_hit.y - t_hit.x;

	vec3 firstValues = vec3(0.0);
	bool notFound = true;

	//samplingDistance = renderDistance / 1000.0;

	vec3 sampligPoint = rayOrigin + t_hit.x * rayDir;
	vec4 color = vec4(0.0);

	vec3 scalingFactor = voxelSpacing * dimensionScaling;
	

	float ii = 0;
	Intersection cut;
	if (cubeCut || sphereCut)
	{
		if (sphereCut)
		{
			vec4 spherePos = (inverseModelViewProjectionMatrix) * vec4(0.0,0.0,0.7, 1.0);
			spherePos /= spherePos.w;
			vec4 sphereCurve = inverseModelViewProjectionMatrix * vec4(sphereCutRadius, 0.0,0.0,0.0);
			float sphereRad = length(sphereCurve.xyz);
			cut = calcSphereIntersection(sphereRad, spherePos.xyz, rayOrigin, rayDir);
		}
		else 
		{
			vec4 box_min = vec4(-0.5, -0.5, -0.5, 1.0);
			//box_min /= box_min.w;
		
			vec4 box_max = vec4(box_min.xyz + cubeCutSize, 1.0);
			//box_max /= box_max.w;

			cut = calcBoxIntersection(box_min.xyz, box_max.xyz, rayOrigin, rayDir);

		}


	
		if (cut.hit)
		{
			if (showInFront)
			{
				fragColor = vec4(diffuseComponent(lightPosition, cut.enterPoint, -cut.normal, vec3(0.0,1.0,0.0)) ,1.0);
				gl_FragDepth = 0.99;
				return;
			}
			if (length(cut.enterPoint - rayOrigin) <= length(sampligPoint - rayOrigin))
			{
				if (length(cut.exitPoint - rayOrigin) <= length(t_hit.y * rayDir) && t_hit.x <= t_hit.y ) 
				{
					if (length(cut.exitPoint - rayOrigin) >= length(sampligPoint - rayOrigin)) 
					{
						if (showCut)
						{
							fragColor = vec4(diffuseComponent(lightPosition, cut.exitPoint, cut.normal, vec3(0.0,1.0,0.0)),1.0);
							gl_FragDepth = 0.99;
							return;
						}
						ii = length(cut.exitPoint - sampligPoint);
						sampligPoint = cut.exitPoint;
						cut.hit = false; // to not have to check intersection again
					}
				}
				else if (t_hit.x <= t_hit.y) 
				{
					fragColor = vec4(backgroundColorVector,1.0);
					gl_FragDepth = 0.99;
					return;
				}
			}
		}
	}
	

	if (t_hit.x > t_hit.y) {
		fragColor = vec4(backgroundColorVector,1.0);
		gl_FragDepth = 0.99;
		return;
	}


	for (float i = ii; i <= renderDistance; i += samplingDistance) 
	{
		if ((sphereCut || cubeCut) && cut.hit)
		{
			if (length(cut.enterPoint - rayOrigin) <= length(sampligPoint - rayOrigin))
			{
				if (length(cut.exitPoint - rayOrigin) <= length(t_hit.y * rayDir)) 
				{
					if (length(cut.exitPoint - rayOrigin) >= length(sampligPoint - rayOrigin)) 
					{
						if (showCut)
						{
							fragColor = vec4(0.0,1.0,0.0,1.0);
							gl_FragDepth = 0.99;
							return;
						}
						i = length(cut.exitPoint - sampligPoint);
						sampligPoint = cut.exitPoint;
						cut.hit = false; // to not have to check intersection again
					}
				}
				else 
				{
					break;
				}
			}
		}

		// With no special scaling, we would just do 0.5 + sampligPoint/2 to map from [-1,1] to [0,1] (ie voxelSpacing and dimensionScaling are equal to 1)
		// With different dimension scales and voxel spacing we need to scale by these factors also. Note both are in interval [0,1].
		vec3 scaledSamplePoint = 0.5 + sampligPoint / (scalingFactor * 2.0);
		vec4 densityAndGradient = texture(volumeTexture, scaledSamplePoint);

		//if (densityAndGradient.x <= 0.1)
		//{
		//	sampligPoint += rayDir * samplingDistance;
		//	continue;
		//}

		vec4 tfColor = texture(transferFunction, vec2(densityAndGradient.r,0.5));

		float x = 0.5*(texture(volumeTexture, scalePoint(vec3(sampligPoint.x + voxelDimsInTexCoord.x, sampligPoint.yz))).r - (texture(volumeTexture, scalePoint(vec3(sampligPoint.x - voxelDimsInTexCoord.x, sampligPoint.yz))).r));
		float y = 0.5*(texture(volumeTexture, scalePoint(vec3(sampligPoint.x, sampligPoint.y + voxelDimsInTexCoord.y, sampligPoint.z))).r - (texture(volumeTexture, scalePoint(vec3(sampligPoint.x, sampligPoint.y - voxelDimsInTexCoord.y, sampligPoint.z))).r));
		float z = 0.5*(texture(volumeTexture, scalePoint(vec3(sampligPoint.xy, sampligPoint.z + voxelDimsInTexCoord.z))).r - (texture(volumeTexture, scalePoint(vec3(sampligPoint.xy, sampligPoint.z - voxelDimsInTexCoord.z))).r));
		
		//float x = 0.5*(texture(volumeTexture, vec3(scaledSamplePoint.x+voxelDimsInTexCoord.x, scaledSamplePoint.y, scaledSamplePoint.z)).r - (texture(volumeTexture, vec3(scaledSamplePoint.x-voxelDimsInTexCoord.x, scaledSamplePoint.y, scaledSamplePoint.z)).r));
		//float y = 0.5*(texture(volumeTexture, vec3(scaledSamplePoint.x, scaledSamplePoint.y+voxelDimsInTexCoord.y, scaledSamplePoint.z)).r - (texture(volumeTexture, vec3(scaledSamplePoint.x, scaledSamplePoint.y-voxelDimsInTexCoord.y, scaledSamplePoint.z)).r));
		//float z = 0.5*(texture(volumeTexture, vec3(scaledSamplePoint.x, scaledSamplePoint.y, scaledSamplePoint.z+voxelDimsInTexCoord.z)).r - (texture(volumeTexture, vec3(scaledSamplePoint.x, scaledSamplePoint.y, scaledSamplePoint.z-voxelDimsInTexCoord.z)).r));
		//vec3 phong = (normalize(densityAndGradient.yzw) + 1) * 0.5;
		//vec3 phong = (normalize(vec3(x,y,z)) + 1) * 0.5;
		
		
		//vec3 phong = diffuseComponent(light.xyz, sampligPoint, normalize(densityAndGradient.yzw), pfColor.rgb);


		//vec3 phong = diffuseComponent((inverseModelViewProjectionMatrix * vec4(vec3(0.0), 1.0)).xyz, sampligPoint, normalize(densityAndGradient.yzw), pfColor.rgb);
		vec3 phong = diffuseComponent(lightPosition, sampligPoint, normalize(vec3(x,y,z)), tfColor.rgb);
		phong += tfColor.rgb * 0.2;
		color.rgb += (1.0 - color.a) * tfColor.a * phong;
		color.a += (1.0 - color.a) * tfColor.a;
		notFound = false;


		if (color.a >= 0.95) 
		{
			break;
		}
		sampligPoint += rayDir * samplingDistance;
	}

	if (notFound)
	{
		fragColor = vec4(backgroundColorVector, 1.0);
		gl_FragDepth = 0.99;
	}
	else
	{
		if (color.a != 1.0)
		{
			color += vec4(backgroundColorVector * (1.0 - color.a), 1.0);
		}
		//vec3 lightpos = vec3(0.0);//(modelViewProjectionMatrix * vec4(vec3(0.0),1.0)).xyz;
		//vec3 t = sampligPoint;//(modelViewProjectionMatrix * vec4(sampligPoint, 1.0)).xyz;//(inverseModelViewProjectionMatrix * vec4(normalize(sampligPoint),1.0)).xyz;//
		//vec3 tt = firstValues;//(modelViewProjectionMatrix * vec4(firstValues, 1.0)).xyz;//(inverseModelViewProjectionMatrix * vec4(firstValues, 1.0)).xyz;
		fragColor = color;//vec4(specularComponent(lightpos, t, tt) + diffuseComponent(lightpos, t, tt) + vec3(0.1), 1.0);//vec4(firstValues,1.0);
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