# Renderer
A 3D OpenGL Physically Based Renderer written in C++.

![Village](https://github.com/user-attachments/assets/45f6fb97-62dc-4435-b2b2-9320fac69945)
![House](https://github.com/user-attachments/assets/01192669-82f7-42df-ab93-0a3f002496c0)

### Features
* Physically Based Rendering Shader
* Directional Light Shadow Mapping
* Parent-Child Hierarchy
* Custom Entity Component System
* Model Importing via [assimp](https://github.com/assimp/assimp)
* Support for Skyboxes using Cubemaps
* FPS Style Camera for Viewing the Scene

### Physically Based Shader
The PBR shader derives from microfacet theory and models dielectric and metallic materials isotropically.
The rendering equation implemented in general form is
$$f(l, v) = diffuse + {D(\theta_h)F(\theta_d)G(\theta_l, \theta_v) \over 4cos\theta_lcos\theta_v}$$ 
where 
* $D(\theta_h) =$ Normal Distribution Function
* $F(\theta_d) =$ Fresnel Function
* $G(\theta_l) =$ Geometric Attenuation Function

##### Diffuse
For diffuse reflections where light hits a surface and scatters evenly in all directions, the Lambertion diffuse is a simple and common model. 
$$Diffuse = color * cos\theta_l$$ 
$$\text{where } cos\theta_l=\omega_l \cdot N$$
The lambertion diffuse takes the base color of a fragment and multiplies it by a scalar based on how much the fragment's normal aligns with the  light direction. A closely aligned normal will make the fragment brighter as it is in direct light and continue to get darker as the angle of incidence increases until normals at $90^\circ$ or greater become black.

This produces nice results but due to the nature of the dot product the edges appear a bit too dark for my liking. I decided to use the Disney diffuse which builds ontop of the labertion diffuse but introduces a Fresnel factor that makes the edges of rough surfaces appear brighter. 
$$f_d = baseColor\over\pi (1 + (F_D90 - 1)(1 - cos\theta_l))$$
While not necessarly physicaly accurate, I find it to look nicer without compromising realism. The creator of this mode, Brent Burley, states that "a possible explanation for this effect may be that, for rough surfaces, light enters and exits the sides of micro-surface features, causing an increase in refraction at grazing angles." Which is convincing enough for me I guess.


### About
This is currently a hobby project and is still a work in progress. My goal is to continue development and turn this renderer into a small, personal game engine, just for fun.
