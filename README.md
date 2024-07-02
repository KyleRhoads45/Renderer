# Renderer
A 3D OpenGL Physically Based Renderer written in C++.

![Demo Scene](https://user-images.githubusercontent.com/52022661/225071513-5783470a-4a70-453e-94af-40b710fdfee6.png)

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
$$Diffuse = color * cos\theta_l /text{where} cos\theta_l = /omega_l /cdot N$$



### About
This is currently a hobby project and is still a work in progress. My goal is to continue development and turn this renderer into a small, personal game engine, just for fun.
