# Tarea 1

### Dependencias
* OpenCV

### Compilación
```
cmake .
make
```

### Ejecución
```
cd builddir
./GAMMA [-m1 | -m2] -v gamma [-f x y w h] [-c r g b]
./GAMMA [-m1 | -m2] -i imagen gamma [-f x y w h] [-c r g b]
```
m1: Utilizar tabla precalculada para la transformaci´on gamma.
m2: Implementar el procesamiento pixel a pixel de gamma (sin tabla).
-v: usar webcam.
-i: usar imagen.
image: Ruta de imagen a procesar.
gamma: Nivel de Gamma. Debe ser un valor real, mayor que 0.
-f: (Optional) Define ´area a la que aplica gamma. Resto es borde.
Se define rect´angulo posicion (x,y) y tama~no w X h.
% Si no se define, se usa imagen completa.
-c: (Optional) Color (r,g,b) del borde. Si no se define,
se usa negro.
