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

$$G = G(\theta_l)G(\theta_v) \text{  where } G(\theta) = \frac{\theta}{\theta(1 - k) + k}$$

#### Fresnel Function
This function describes the phenomenon that surfaces demonstrate greater reflectivity when viewed at grazing angles. The Fresnel-Schlick approximation is used as it is computationally efficient for real-time rendering while giving realistic results.

$$F = F_0 + (1 - F_0)(1 - cos\theta_d)^5 \text{  where } cos\theta_d = h \cdot v$$
