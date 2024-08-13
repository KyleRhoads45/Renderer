# Renderer
A 3D OpenGL Physically Based Renderer written in C++.

![Village](https://github.com/user-attachments/assets/45f6fb97-62dc-4435-b2b2-9320fac69945)
![House](https://github.com/user-attachments/assets/01192669-82f7-42df-ab93-0a3f002496c0)

### Features
---
* [Physically Based Rendering Shader](#phsically-based-shader)
* [Soft Shadows](#shadows)
* Model Importing
* Custom Entity Component System
* IMGUI Editor
* Scene Picking
* HDR with Tonemapping
* Post Processing
* Parent-Child Hierarchy
* Support for Skyboxes using Cubemaps
* FPS Style Camera for Viewing the Scene

### Physically Based Shader
---
The PBR shader derives from microfacet theory and models dielectric and metallic materials isotropically.
The rendering equation implemented in general form is

$$f(l, v) = diffuse + {D(\theta_h)F(\theta_d)G(\theta_l, \theta_v) \over 4cos\theta_lcos\theta_v}$$

where 
* $D(\theta_h) =$ Normal Distribution Function
* $F(\theta_d) =$ Fresnel Function
* $G(\theta_l) =$ Geometric Attenuation Function

#### Diffuse
For diffuse reflections where light hits a surface and scatters evenly in all directions, the Lambertion diffuse is a simple and common model.

$$Diffuse = color * cos\theta_l \text{  where } cos\theta_l=\omega_l \cdot N$$

This produces nice results but due to the nature of the dot product the edges appear a bit too dark for my liking. I decided to use the Disney diffuse which builds ontop of the labertion diffuse but introduces a Fresnel factor that makes the edges of rough surfaces appear brighter.

$$f_d = \frac {baseColor}{\pi} (1 + (F_D90 - 1)(1 - cos\theta_l))$$

While not necessarly physicaly accurate, I find it to look nicer without compromising realism. The creator of this mode, Brent Burley, states that "a possible explanation for this effect may be that, for rough surfaces, light enters and exits the sides of micro-surface features, causing an increase in refraction at grazing angles." Which is convincing enough for me.

#### Normal Distribution Function
Since microfacets are too small for computers to work with, we use a distribution function that represents the probability that incoming light hits a microfacet whose normal aligns with the halfway vector, resulting in a perfect reflection. For this function I chose GGX as it is widely adopted and I prefer the look of the longer tail it offers as oppossed to something like Beckmann. 

$$GGX = \frac{\alpha^2}{\pi((\alpha^2 - 1)cos^2\theta_h + 1)^2} \text{  where } \alpha = roughness^2 \text{, } cos\theta_h = n \cdot h$$

#### Geometric Attenuation Function
For this function I chose to use the Smith method paired with the Schlick-GGX geometry function as it is complimentary to the GGX distribution function. The Smith method accounts for both geometric obstruction and shadowing by calculating the geometry function separately for the light and view directions.

$$G = G(\theta_l)G(\theta_v) \text{where } G(\theta) = \frac{\theta}{\theta(1 - k) + k}$$

#### Fresnel Function
This function describes the phenomenon that surfaces demonstrate greater reflectivity when viewed at grazing angles. The Fresnel-Schlick approximation is used as it is computationally efficient for real-time rendering while giving realistic results.

$$F = F_0 + (1 - F_0)(1 - cos\theta_d)^5 \text{where } cos\theta_d = h \cdot v$$

### Shadows
---
The renderer implements a soft shadow technique called [Efficient Soft-Edged Shadows](https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using) and has corrections for shadow shimmering artifacts while the camera is translating or rotating around the scene.

#### Soft Shadows
The directional light shadows are rendered in a typical fassion, where the scene is first rendered from the light's perspective with a specific shader that writes the depth values into a framebuffer called the shadow map (depth map). Then when rendering the scene normally, in the fragment shader we compare the depth values of the current fragment's position in light space, with its corresponding depth value from the shadow map. If the current fragment's depth is greater than the depth found in the shadow map, then the fragment is considered to be in shadow. This produces shadows with harsh, jagged edges due to multiple fragments sharing the same pixel in the shadow map.

![JaggedShadowsSmall](https://github.com/user-attachments/assets/8d7ef0b9-85c7-428e-8064-6e0f504d5ae9)

To achieve soft shadows, the fragment shader takes multiple shadow map samples around the current fragment and averages the shadow values together which produces a gradient along the edges. However, the way we sample nearby texels matters considerably as a uniform kernal will still produce jagged edges, albeit less harsh than before, but still bad. To fix this, we introduce noise by sampling random texels close to the fragment. We can't use the same randomness pattern for each fragment though, since that will produce similar jagged results, so we need many different random sampling patterns to break up repetition.

For this we have an N x N grid with each cell containing a unique M x M texture of randomized sampling offsets to apply to the fragment's light position when sampling the shadow map. These offset textures are stored in one 3D texture of $M^2 x N x N$ dimension, where each row is a complete set of sampling offsets. Each fragment gets mapped into one of the grid cells based on its screen space position by **mod(gl_FragCoord.xy, vec2(N))**. Once we know which fragment the cell falls into, we use that as the Y and Z index into the 3D texture while we iterate the X index in the range of $[0, M^2)$ to get our offsets.

Now for the efficent part of Efficient Soft-Edged Shadows. Most fragments don't lay on a shadow's edge which means a lot of the shadow map sampling is unecessarry overhead. This technique allows for early branching when sampling by having the offsets be organized as a series of decreasing rings. By first sampling with the outermost ring, we can determine if the fragment lies on a shadow's edge if the average shadow value is not 0 (all shadow) or 1 (all outside shadow). If the outer ring is either completely in shadow, or completley out of shadow, then all inner rings must be too, so we don't need to sample them. In order to produce these rings, we first create a M x M grid of points and imagine they are all in a 1 x 1 cell. We then apply a random offset $(\alpha_x, \alpha_y)$ where $(\alpha_x, \alpha_y)$ ∈ $[-0.5, 0.5]$ to "jitter" each point within its cell. We then warp each grid column into a ring through the equation, $(x, y) = (√v * cos(2 \pi u), √v * sin(2 \pi u))$ where $(u, v)$ represents the jittered point.

![JitterColored](https://github.com/user-attachments/assets/4c0a20d0-49c5-4016-903d-6f5f112bed77)

The results with a grid size of 16 x 16 and a filter size of 10 x 10. Notice how there is no longer jagged edges or gradient stepping like in the previous image with no PCF filtering.

![SoftShadowsSmall](https://github.com/user-attachments/assets/a663fe3e-77fb-41f2-a34c-5546dd71e936)

#### Correcting Shadow Artifacts
A common problem with shadow mapping is the edges of shadows shimmering as the camera translates and rotates through the scene. The two reasons for shadow shimmering are the shadow map's projection chaniging size during camera rotation and the shadow map not snapping to texel size increments during camera translation.

To fix the camera rotation aspect we find the sphere centered in the camera's frustrum that encompases its entirety and use its diamter as the shadow projection size. This ensures that as the camera rotates, its frustrum will always be inside the sphere and the projection map will remain a constant size. Then to fix the translation aspect, the sphere's center is translated into light space, snapped to the closest texel size increment, then translated back into world space. The code for this is as follows.
```cpp
f32 projSize = CameraSystem::ViewFrustumDiagonal(m_ShadowDist);

glm::vec3 lightDir = Enviroment::Instance()->GetLightDir();
glm::vec3 frustumCenter = CameraSystem::ActiveCamPos() + CameraSystem::ActiveCamForward() * (m_ShadowDist / 2.0f);

// Make the light's view matrix move in texel size increments by snapping the frustum center.
{
  glm::mat4 lightSpaceView = glm::lookAt(glm::vec3(0, 0, 0), lightDir, glm::vec3(0, 1, 0));

  // Convert camera frustum center to light space so its as if we are viewing it from the lights perspective.
  frustumCenter = lightSpaceView * glm::vec4(frustumCenter, 1.0f);

  // Snap the center to the shadow texel grid
  glm::vec3 worldUnitsPerTexel = glm::vec3(projSize / m_TextureSize, projSize / m_TextureSize, 1.0f);
  frustumCenter /= worldUnitsPerTexel;
  frustumCenter = glm::floor(frustumCenter);
  frustumCenter *= worldUnitsPerTexel;

  // Transform it back into world space so its projected properly
  frustumCenter = glm::inverse(lightSpaceView) * glm::vec4(frustumCenter, 1.0f);
}

// Create the light's view matrix with the view position being the center of the camera's frustum.
glm::mat4 view = glm::lookAt(frustumCenter - lightDir, frustumCenter, glm::vec3(0, 1, 0));

f32 halfProjSize = projSize / 2.0f;
glm::mat4 projection = glm::ortho(-halfProjSize, halfProjSize, -halfProjSize, halfProjSize, -projSize, projSize);
```

### Model Importing
---
For importing 3D models I chose to use the [assimp](https://github.com/assimp/assimp) library as its capable of parsing many different file formats into generic data structures that the renderer can use easily. When importing, the necessarry model data for the renderer is parsed into YAML files using the [yaml-cpp](https://github.com/jbeder/yaml-cpp) library. The import process works as follows: assimp first imports and parses the model file. Then, for each material, a YAML file is generated containing all necessary texture paths and parameters required for the renderer's PBR material. Finally, a single YAML file is created for the model containing parent, transform, mesh, and material data for each node within the model. 















