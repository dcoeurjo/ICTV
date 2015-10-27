# Interactive Curvature Tensor Visualization

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

[https://www.youtube.com/watch?v=s65R0osQmYo](https://www.youtube.com/watch?v=s65R0osQmYo)

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


:octocat:

