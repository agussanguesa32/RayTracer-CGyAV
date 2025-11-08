## Ray tracer recursivo - Parcial CG

Proyecto C++ sin frameworks de ray tracing. Genera imagen PPM con rayos primarios, intersecciones con esfera y triangulo, iluminacion Phong, sombras, reflexion y refraccion con Fresnel, mas modos de prueba.

# Demo
<img width="800" height="450" alt="image" src="https://github.com/user-attachments/assets/1e9700b2-1292-4267-9ecc-dec9e2b3fb4c" />


### Estructura de carpetas
- `src/core/`: tipos base
  - `Vec3.h`: vector 3D y utilidades (dot, cross, normalize, reflect, refract, fresnel simple)
  - `Ray.h`: rayo (origen, direccion, at(t))
- `src/geometry/`: primitivas e interfaz
  - `Hittable.h`: interfaz de objeto golpeable y `HitRecord`
  - `Sphere.h`: interseccion por cuadratica
  - `Triangle.h`: interseccion Moller Trumbore
  - `Plane.h`: plano infinito
- `src/materials/`
  - `Material.h`: parametros Phong (Ka, Kd, Ks, shininess), reflectividad, transparencia, ior, fuzz y `emissive`/`castsShadow`
- `src/lights/`
  - `PointLight.h`: luz puntual simple (pos, color, intensidad)
- `src/camera/`
  - `Camera.h`: camara pinhole
- `src/scene/`
  - `Scene.h`: contenedor de objetos y luces, `hit` y `isOccluded`
- `src/renderer/`
  - `Integrator.h`: traza recursiva (Phong + sombras + reflexion/refraccion con control de profundidad y atenuacion por distancia)
  - `Renderer.h`: bucle de imagen con spp
- `src/utils/`
  - `Random.h`: rng simple
  - `ImageWriterPPM.h`: salida PPM (P3) con gamma opcional
- `src/main.cpp`: parseo de CLI, escenas de prueba y render
- `docs/`: consigna/roadmap
- `img/`: imagenes generadas

### Que hace cada parte (resumen)
- Camara: emite rayos primarios por pixel; AA por spp; gamma en modo final.
- Geometria: esfera, triangulo y plano con manejo de front face y epsilon.
- Materiales: lambertiano, metal y dielectrico; soporte de emision para pantallas de lamparas.
- Iluminacion: Phong por luces puntuales; sombras por rayo de sombra; atenuacion 1/(1+k*d^2).
- Recursion: reflexion y refraccion con Fresnel (Schlick) y `maxDepth`.
- Escenas: `final` (habitacion con espejo) y `base` (tres esferas sobre plano).

### Requisitos
- CMake >= 3.15
- Compilador C++ con soporte C++17
 - (opcional PNG) Netpbm (`pnmtopng`) o ImageMagick (`convert`)

### Compilar (importante crear la carpeta build en la raiz primero)
```bash
mkdir build 
cmake -S . -B build
cmake --build build -j
```

### Ejecutar (ejemplos)
- Render final por defecto con resolucion 800x450:
```bash
./build/raytracer --width 800 --height 450 --spp 8 --max-depth 6 --out img/final.ppm
```
- Render directo a PNG (si hay `pnmtopng` o `convert` disponible):
```bash
./build/raytracer --width 800 --height 450 --spp 8 --max-depth 6 --out img/final.png
```
- Vistas de camara en modo final (`--camera frontal|superior|lateral`):
  - Frontal (por defecto):
```bash
./build/raytracer --width 800 --height 450 --spp 8 --max-depth 6 --out img/final_frontal.png
```
  - Superior (vista cenital inclinada para ver el piso y el cono):
```bash
./build/raytracer --width 800 --height 450 --spp 8 --max-depth 6 --camera superior --out img/final_superior.png
```
  - Lateral (desde la izquierda, pared con lampara visible):
```bash
./build/raytracer --width 800 --height 450 --spp 8 --max-depth 6 --camera lateral --out img/final_lateral.png
```
- Escena base (tres esferas sobre plano):
```bash
./build/raytracer --scene base --width 800 --height 450 --spp 8 --max-depth 6 --out img/base_frontal.png
```
- Escena base con otras vistas:
```bash
./build/raytracer --scene base --camera superior --out img/base_superior.png
./build/raytracer --scene base --camera lateral --out img/base_lateral.png
```

### Parametros CLI
- `--width <int>` ancho de imagen
- `--height <int>` alto de imagen
- `--spp <int>` muestras por pixel (AA)
- `--max-depth <int>` profundidad recursiva maxima
- `--scene final|base` escena a renderizar
- `--out <ruta>` archivo de salida (PPM por defecto, PNG si termina en .png)
- `--camera frontal|superior|lateral` preset de camara para el modo final

### Notas
- Imagen PPM P3 (texto) para simplicidad.
- Las pantallas de lamparas usan `emissive` y `castsShadow=false` para justificar la luz sin bloquearla.
- El espejo se modela con dos triangulos y material metalico (reflectividad 1 y fuzz bajo), lo que permite rebotes multiples.
 - Para `.png`, el programa escribe un PPM temporal y lo convierte con `pnmtopng` o `convert` si estan instalados.


