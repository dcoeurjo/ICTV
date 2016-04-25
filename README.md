# Interactive Curvature Tensor Visualization

ICTV (Interactive Curvature Tensor Visualization) is an opensource (GPL) OpenGL tool to visualize large volumetric  data isosurfaces in realtime. This tool also combines the isosurface extraction with a GPU curvature tensor estimation (mean/gaussian curvature, principal curvature directions...) allowing a realtime exploration of the tensor with different parameter values.

This work has been presented at [DGCI 2016](http://dgci2016.univ-nantes.fr). Please cite this project as follows:

```bibtex
@INBOOK {ICTV16,
    author    = "H\'el\`ene Perrier and J\'er\'emy Levallois and David Coeurjolly and {Jean-Philippe} Farrugia and {Jean-Claude} Iehl and {Jacques-Olivier} Lachaud",
    title     = "Discrete Geometry for Computer Imagery: 19th IAPR International Conference, DGCI 2016, Nantes, France, April 18-20, 2016. Proceedings",
    chapter   = "Interactive Curvature Tensor Visualization on Digital Surfaces",
    publisher = "Springer International Publishing",
    year      = "2016",
    editor    = "Nicolas Normand and Jeanpierre Gu{\'e}don and Florent Autrusseau",
    pages     = "282--294",
    isbn      = "978-3-319-32360-2",
    doi       = "10.1007/978-3-319-32360-2_22",
    url       = "http://dx.doi.org/10.1007/978-3-319-32360-2_22"
}
```



This tool requires OpenGL 4.4 (see below for other library dependencies). 


### Build & Run

```sh
cd src/GKIT
premake4 gmake
make ICTV
./ICTV <data> <type> <size> [light]
```

* \<type\>:	1 - .raw files  
			2 - Enter a piece of glsl code filling a variable h with the implicit fonction to render.
				x, y, and z are available variables containing the position of the vertex in [-10:10].
				t is a variable containing the time elapsed is seconds.
				A point (x,y,z) is considered inside if h < 0.
* \<size\>:	resolution of the dataset

* \<light\>: If a fourth argument is given (whatever its value), the application is run in light mode. 
			 It requires less GPU memory but does not allow to export the data

#### Example

```sh
#loading the bunny
./ICTV ../../data/raw/bunny_border_67.raw 1 67
#loading the moving sinus cardinal
./ICTV "float d=(abs(sin(0.5*t))+0.5) * sqrt(x*x+z*z)+0.01; h=y - (10*(sin(d)/d));" 2 128
#loading the octa flower in light mode
./ICTV ../../data/raw/octa-flower-130_border.raw 1 130 1
```

#### Dependencies

* SDL2
* SDL2_image
* SDL2_ttf
* openexr
* glew

### Video

* Preview: [https://www.youtube.com/watch?v=JWszppL8aP4](https://www.youtube.com/watch?v=JWszppL8aP4)
* Large dataset exploration: [https://www.youtube.com/watch?v=1mUPesJ0IyI](https://www.youtube.com/watch?v=1mUPesJ0IyI)

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
