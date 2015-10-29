# Interactive Curvature Tensor Visualization

ICTV (Interactive Curvature Tensor Visualization) is an opensource (GPL) OpenGL tool to visualize large volumetric  data isosurfaces in realtime. This tool also combines the isosurface extraction with a GPU curvature tensor estimation (mean/gaussian curvature, principal curvature directions...) allowing a realtime exploration of the tensor with different parameter values.

This tool requires OpenGL 4.4 (see below for other library dependencies). 


### Build & Run

```sh
cd src/GKIT
premake4 gmake
make vizo
./vizo <data> 1 <size>
```
* \<data\> - any other .raw file
* \<size\> - object's bounding box.

#### Example

```sh
./vizo ../../data/raw/octa-flower-130_border.raw 1 130
```

#### Dependencies

* SDL2
* SDL2_image
* SDL2_ttf
* openexr
* glew

### Video

[https://www.youtube.com/watch?v=JWszppL8aP4](https://www.youtube.com/watch?v=JWszppL8aP4)

### Preview

<img src="http://i.imgur.com/m72QeV9.gif" alt="Mean curvature" />

<img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_mean.png" alt="Mean curvature" style="width: 200px;"/> | <img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_gaussian.png" alt="Gaussian curvature" style="width: 200px;"/> | <img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_k1.png" alt="First principal curvature" style="width: 200px;"/> | <img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_k2.png" alt="Second principal curvature" style="width: 200px;"/>
------------ | ------------- | ------------- | ------------- 
Mean curvature | Gaussian curvature | First principal curvature | Second principal curvature
<img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_dir_min.png" alt="First principal direction field" style="width: 200px;"/> | <img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_dir_max.png" alt="Second principal direction field" style="width: 200px;"/> | <img src="https://raw.github.com/dcoeurjo/ICTV/master/latex/figs/EG_dragon_normal.png" alt="Normal vector field" style="width: 200px;"/> | 
First principal direction field | Second principal direction field | Normal vector field | 

### Credits

*	Hélène Perrier [@Logarithme](https://github.com/Logarithme)
*	Jérémy Levallois [@jlevallois](https://github.com/jlevallois)
*	David Coeurjolly [@dcoeurjo](https://github.com/dcoeurjo)
*	Jean-Philippe Farrugia [@jpfarrug](https://github.com/jpfarrug)
*	Jean-Claude Iehl [@jciehl](https://github.com/jciehl)
*	Jacques-Olivier Lachaud [@JacquesOlivierLachaud](https://github.com/JacquesOlivierLachaud)

### License

* Source code ([/src](https://github.com/dcoeurjo/ICTV/tree/master/src) and  [/tools](https://github.com/dcoeurjo/ICTV/tree/master/tools))

>	**GNU General Public License**
>	
>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>    
>    http://www.gnu.org/licenses/gpl-3.0.en.html

* Article and figures ([/latex](https://github.com/dcoeurjo/ICTV/tree/master/latex))

> **Creative Commons CC BY-NC-SA 2.0**
> 
>    https://creativecommons.org/licenses/by-nc-sa/2.0/


:octocat:
