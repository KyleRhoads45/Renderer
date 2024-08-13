# Renderer
A 3D OpenGL Physically Based Renderer written in C++.

![Village](https://github.com/user-attachments/assets/45f6fb97-62dc-4435-b2b2-9320fac69945)
![House](https://github.com/user-attachments/assets/01192669-82f7-42df-ab93-0a3f002496c0)

### Features
---
* Physically Based Rendering Shader
* Directional Light Shadow Mapping
* Parent-Child Hierarchy
* Custom Entity Component System
* Model Importing via [assimp](https://github.com/assimp/assimp)
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
The renderer implements a soft shadow technique called [Efficient Soft-Edged Shadows](https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using) and has corrections for shadow "swimming" artifacts while the camera is translating or rotating around the scene.

#### Soft Shadows
The directional light shadows are rendered in a typical fassion, where the scene is first rendered from the light's perspective with a specific shader that writes the depth values into a framebuffer called the shadow map (depth map). Then when rendering the scene normally, in the fragment shader we compare the depth values of the current fragment's position in light space, with its corresponding depth value from the shadow map. If the current fragment's depth is greater than the depth found in the shadow map, then the fragment is considered to be in shadow. This produces shadows with harsh, jagged edges due to multiple fragments sharing the same pixel in the shadow map.

![JaggedShadowsSmall](https://github.com/user-attachments/assets/8d7ef0b9-85c7-428e-8064-6e0f504d5ae9)

To achieve soft shadows, the fragment shader takes multiple shadow map samples around the current fragment and averages the shadow values together which produces a gradient along the edges. However, the way we sample nearby texels matters considerably as a uniform kernal will still produce jagged edges, albeit less harsh than before, but still bad. To fix this, we introduce noise by sampling random texels close to the fragment. We can't use the same randomness pattern for each fragment though, since that will produce similar jagged results, so we need many different random sampling patterns to break up repetition.

For this we have an N x N grid with each cell containing a unique M x M texture of randomized sampling offsets to apply to the fragment's light position when sampling the shadow map. These offset textures are stored in one 3D texture of $M^2 x N x N$ dimension, where each row is a complete set of sampling offsets. Each fragment gets mapped into one of the grid cells based on its screen space position by **mod(gl_FragCoord.xy, vec2(N))**. Once we know which fragment the cell falls into, we use that as the Y and Z index into the 3D texture while we iterate the X index in the range of $[0, M^2)$ to get our offsets.

Now for the efficent part of Efficient Soft-Edged Shadows. Most fragments don't lay on a shadow's edge which means a lot of the shadow map sampling is unecessarry overhead. This technique allows for early branching when sampling by having the offsets be organized as a series of decreasing rings. By first sampling with the outermost ring, we can determine if the fragment lies on a shadow's edge if the average shadow value is not 0 (all shadow) or 1 (all outside shadow). If the outer ring is either completely in shadow, or completley out of shadow, then all inner rings must be too, so we don't need to sample them. In order to produce these rings, we first create a M x M grid of points and imagine they are all in a 1 x 1 cell. We then apply a random offset $(\alpha_x, \alpha_y)$ where $(\alpha_x, \alpha_y)$ ∈ $[-0.5, 0.5]$ to "jitter" each point within its cell. We then warp each grid column into a ring through the equation, $(x, y) = (√v * cos(2 \pi u), √v * sin(2 \pi u)$ where $(u, v)$ represents the jittered point.

![JitterColored](https://github.com/user-attachments/assets/4c0a20d0-49c5-4016-903d-6f5f112bed77)

