#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "core/Vec3.h"
#include "core/Ray.h"
#include "utils/ImageWriterPPM.h"
#include "utils/ImageWriterAuto.h"
#include "geometry/Sphere.h"
#include "geometry/Triangle.h"
#include "geometry/Plane.h"
#include "materials/Material.h"
#include "lights/PointLight.h"
#include "camera/Camera.h"
#include "scene/Scene.h"
#include "renderer/Renderer.h"

using namespace rt;

struct Args {
  int width = 800;
  int height = 450;
  int spp = 1;
  int maxDepth = 6;
  std::string scene = "final"; // "final" o "base"
  std::string out = "img/output.ppm";
  std::string camera = "frontal"; // "frontal" | "superior" | "lateral"
};

static Args parseArgs(int argc, char** argv) {
  Args a;
  for (int i = 1; i < argc; ++i) {
    std::string k = argv[i];
    auto readInt = [&](int& dst){ if (i+1 < argc) dst = std::stoi(argv[++i]); };
    auto readStr = [&](std::string& dst){ if (i+1 < argc) dst = std::string(argv[++i]); };
    if (k == "--width") readInt(a.width);
    else if (k == "--height") readInt(a.height);
    else if (k == "--spp") readInt(a.spp);
    else if (k == "--max-depth") readInt(a.maxDepth);
    else if (k == "--scene") readStr(a.scene);
    else if (k == "--out") readStr(a.out);
    else if (k == "--camera") readStr(a.camera);
  }
  return a;
}

// Escena base: plano y tres esferas (difusa, metal, dielectrico)
static void buildBaseScene(Scene& scene, Camera*& cam, int width, int height, const std::string& cameraView) {
  // Materiales simples
  auto sueloMat = std::make_shared<Lambertian>(Vec3{0.75, 0.75, 0.75});
  auto difusa = std::make_shared<Lambertian>(Vec3{0.80, 0.25, 0.25});
  auto metal = std::make_shared<Metal>(Vec3{0.90, 0.90, 0.90}, 0.02, 1.0);
  auto vidrio = std::make_shared<Dielectric>(1.5);
  // Panel emisivo para que se vea brillante a traves del vidrio
  auto rojo = std::make_shared<Lambertian>(Vec3{0.5, 0.1, 0.1});
  rojo->emissive = Vec3{0.75, 0.1, 0.1}; // emite luz roja
  auto naranja = std::make_shared<Lambertian>(Vec3{0.5, 0.3, 0.1});
  naranja->emissive = Vec3{0.85, 0.5, 0.1}; // emite luz naranja

  // Planos: suelo y fondo
  scene.addObject(std::make_shared<Plane>(Vec3{0,1,0}, 0.0, sueloMat));      // y=0
  scene.addObject(std::make_shared<Plane>(Vec3{0,0,1}, 4.0, sueloMat));      // z=-4

  // Esferas
  scene.addObject(std::make_shared<Sphere>(Vec3{-0.9, 0.5, -2.4}, 0.5, difusa)); // lambertiana
  scene.addObject(std::make_shared<Sphere>(Vec3{ 0.0, 0.5, -2.8}, 0.5, metal));  // espejo
  scene.addObject(std::make_shared<Sphere>(Vec3{ 1.0, 0.5, -2.2}, 0.5, vidrio)); // dielectric

  // Panel de dos colores detras de la esfera de vidrio para verificar refraccion
  // La esfera de vidrio esta en (1.0, 0.5, -2.2) con radio 0.5
  // Su parte trasera esta en z = -2.2 - 0.5 = -2.7
  // El panel debe estar MAS ATRAS que -2.7, pero no muy lejos
  double zPanel = -3.4;
  // Panel mas grande y centrado en la esfera de vidrio
  // Mitad rojo / mitad amarillo: xM es el punto medio entre xL y xR
  // Lo desplazamos un poco hacia la derecha manteniendo el ancho
  double dx = 0.30;
  double xL = 0.2 + dx, xR = 1.8 + dx;
  double xM = 0.5 * (xL + xR);
  double yB = -0.2, yT = 1.5;
  // rectangulo izquierdo (rojo)
  scene.addObject(std::make_shared<Triangle>(Vec3{xL, yB, zPanel}, Vec3{xM, yB, zPanel}, Vec3{xL, yT, zPanel}, rojo));
  scene.addObject(std::make_shared<Triangle>(Vec3{xM, yB, zPanel}, Vec3{xM, yT, zPanel}, Vec3{xL, yT, zPanel}, rojo));
  // rectangulo derecho (naranja)
  scene.addObject(std::make_shared<Triangle>(Vec3{xM, yB, zPanel}, Vec3{xR, yB, zPanel}, Vec3{xM, yT, zPanel}, naranja));
  scene.addObject(std::make_shared<Triangle>(Vec3{xR, yB, zPanel}, Vec3{xR, yT, zPanel}, Vec3{xM, yT, zPanel}, naranja));

  // Luces
  scene.lights.push_back(PointLight{Vec3{0.0, 2.2, -2.2}, Vec3{1,1,1}, 0.9});
  scene.lights.push_back(PointLight{Vec3{-1.6, 1.6, -3.2}, Vec3{1.0, 0.95, 0.9}, 0.4});

  // Camara: reutilizamos presets para coherencia
  Vec3 lookAt{0.0, 0.4, -2.6};
  Vec3 lookFrom;
  Vec3 vup;
  auto toLower = [](std::string s){ for (auto& c : s) c = (char)tolower(c); return s; };
  std::string view = toLower(cameraView);
  double fovDeg = 50.0;
  if (view == "frontal" || view == "front") {
    lookFrom = Vec3{0.0, 1.0, 1.2};
    vup = Vec3{0.0, 1.0, 0.0};
  } else if (view == "superior" || view == "top") {
    lookFrom = Vec3{0.0, 2.2, 0.6};
    lookAt = Vec3{0.0, 0.25, -2.4};
    vup = Vec3{0.0, 1.0, 0.0};
    fovDeg = 70.0;
  } else if (view == "lateral" || view == "side") {
    lookFrom = Vec3{-1.8, 1.0, -2.6};
    vup = Vec3{0.0, 1.0, 0.0};
    fovDeg = 65.0;
  } else {
    lookFrom = Vec3{0.0, 1.0, 1.2};
    vup = Vec3{0.0, 1.0, 0.0};
  }
  double aspect = (double)width / (double)height;
  cam = new Camera(lookFrom, lookAt, vup, fovDeg, aspect);
  // Fondo mas oscuro para que la refraccion del panel sea mas visible
  scene.background = Vec3{0.2, 0.2, 0.3};
}

// Escena final: habitacion de madera con espejo
static void buildFinalScene(Scene& scene, Camera*& cam, int width, int height, const std::string& cameraView) {
  // Materiales
  auto madera = std::make_shared<Lambertian>(Vec3{0.55, 0.36, 0.22});
  auto marco = std::make_shared<Lambertian>(Vec3{0.05, 0.05, 0.05});
  auto espejo = std::make_shared<Metal>(Vec3{0.95, 0.95, 0.95}, 0.02, 1.0);
  auto difRoja = std::make_shared<Lambertian>(Vec3{0.80, 0.25, 0.25});
  auto metalBlanco = std::make_shared<Metal>(Vec3{0.85, 0.85, 0.85}, 0.05, 1.0);
  auto vidrio = std::make_shared<Dielectric>(1.5);
  auto gris = std::make_shared<Lambertian>(Vec3{0.6, 0.6, 0.6});
  auto lampara = std::make_shared<Lambertian>(Vec3{0.95, 0.95, 0.9});
  lampara->emissive = Vec3{0.9, 0.9, 0.85};
  lampara->castsShadow = false;

  // Planos de la habitacion
  // y = 0 (suelo): n=(0,1,0), d=0
  scene.addObject(std::make_shared<Plane>(Vec3{0,1,0}, 0.0, madera));
  // y = 2.5 (techo): n=(0,-1,0), d=2.5
  scene.addObject(std::make_shared<Plane>(Vec3{0,-1,0}, 2.5, madera));
  // x = -2.0 (pared izq): n=(1,0,0), d=2.0
  scene.addObject(std::make_shared<Plane>(Vec3{1,0,0}, 2.0, madera));
  // x = +2.0 (pared der): n=(-1,0,0), d=2.0
  scene.addObject(std::make_shared<Plane>(Vec3{-1,0,0}, 2.0, madera));
  // z = -6.0 (pared fondo): n=(0,0,1), d=6.0
  scene.addObject(std::make_shared<Plane>(Vec3{0,0,1}, 6.0, madera));

  // espejo en pared de fondo
  double zMirror = -5.9995;
  double xL = -1.8, xR = 1.8, yB = 0.2, yT = 2.3;
  scene.addObject(std::make_shared<Triangle>(Vec3{xL, yB, zMirror}, Vec3{xR, yB, zMirror}, Vec3{xL, yT, zMirror}, espejo));
  scene.addObject(std::make_shared<Triangle>(Vec3{xR, yB, zMirror}, Vec3{xR, yT, zMirror}, Vec3{xL, yT, zMirror}, espejo));

  // marcos del espejo
  double zFrame = -5.9993;
  // marco izquierdo: x in [-2.0, xL], y in [0.0, 2.5]
  scene.addObject(std::make_shared<Triangle>(Vec3{-2.0, 0.0, zFrame}, Vec3{xL, 0.0, zFrame}, Vec3{-2.0, 2.5, zFrame}, marco));
  scene.addObject(std::make_shared<Triangle>(Vec3{xL, 0.0, zFrame}, Vec3{xL, 2.5, zFrame}, Vec3{-2.0, 2.5, zFrame}, marco));
  // marco derecho: x in [xR, 2.0]
  scene.addObject(std::make_shared<Triangle>(Vec3{xR, 0.0, zFrame}, Vec3{2.0, 0.0, zFrame}, Vec3{xR, 2.5, zFrame}, marco));
  scene.addObject(std::make_shared<Triangle>(Vec3{2.0, 0.0, zFrame}, Vec3{2.0, 2.5, zFrame}, Vec3{xR, 2.5, zFrame}, marco));
  // marco inferior: y in [0.0, yB], x in [xL, xR]
  scene.addObject(std::make_shared<Triangle>(Vec3{xL, 0.0, zFrame}, Vec3{xR, 0.0, zFrame}, Vec3{xL, yB, zFrame}, marco));
  scene.addObject(std::make_shared<Triangle>(Vec3{xR, 0.0, zFrame}, Vec3{xR, yB, zFrame}, Vec3{xL, yB, zFrame}, marco));
  // marco superior: y in [yT, 2.5], x in [xL, xR]
  scene.addObject(std::make_shared<Triangle>(Vec3{xL, yT, zFrame}, Vec3{xR, yT, zFrame}, Vec3{xL, 2.5, zFrame}, marco));
  scene.addObject(std::make_shared<Triangle>(Vec3{xR, yT, zFrame}, Vec3{xR, 2.5, zFrame}, Vec3{xL, 2.5, zFrame}, marco));

  // lampara de techo
  double yLamp = 2.30;
  double xl0 = -0.35, xr0 = 0.35, zf0 = -3.6, zn0 = -2.8;
  scene.addObject(std::make_shared<Triangle>(Vec3{xl0, yLamp, zf0}, Vec3{xr0, yLamp, zf0}, Vec3{xl0, yLamp, zn0}, lampara));
  scene.addObject(std::make_shared<Triangle>(Vec3{xr0, yLamp, zf0}, Vec3{xr0, yLamp, zn0}, Vec3{xl0, yLamp, zn0}, lampara));

  // lampara lateral
  double xLamp2 = -1.9993;
  double yL2b = 1.1, yL2t = 1.7;
  double zL2n = -2.6, zL2f = -3.2;
  scene.addObject(std::make_shared<Triangle>(Vec3{xLamp2, yL2b, zL2n}, Vec3{xLamp2, yL2t, zL2n}, Vec3{xLamp2, yL2b, zL2f}, lampara));
  scene.addObject(std::make_shared<Triangle>(Vec3{xLamp2, yL2t, zL2n}, Vec3{xLamp2, yL2t, zL2f}, Vec3{xLamp2, yL2b, zL2f}, lampara));

  // esferas
  scene.addObject(std::make_shared<Sphere>(Vec3{-0.8, 0.5, -2.2}, 0.5, difRoja));
  // metal especular
  metalBlanco->reflectivity = 1.0;
  scene.addObject(std::make_shared<Sphere>(Vec3{1.1, 0.5, -2.8}, 0.5, metalBlanco));

  // triangulos adicionales
  // triangulo apoyado en el piso, rotado levemente para ver dos caras (una especie de tetraedro)
  Vec3 tA{-0.35, 0.0, -1.42}; // base izquierda en el piso
  Vec3 tB{ 0.35, 0.0, -1.55}; // base derecha en el piso
  Vec3 tC{ 0.00, 0.58, -1.50}; // punta superior
  scene.addObject(std::make_shared<Triangle>(tA, tB, tC, gris));
  // segunda cara, base invertida para formar la punta
  Vec3 tB2{-0.35, 0.0, -1.55};
  Vec3 tA2{ 0.35, 0.0, -1.42};
  scene.addObject(std::make_shared<Triangle>(tA2, tB2, tC, gris));

  // luz puntual principal (techo)
  scene.lights.push_back(PointLight{Vec3{0.0, 2.22, -3.2}, Vec3{1,1,1}, 0.9});
  // segunda luz puntual, alineada con la lampara de pared
  scene.lights.push_back(PointLight{Vec3{-1.90, 1.40, -2.90}, Vec3{1.0, 0.9, 0.8}, 0.4});

  // camara pinhole
  Vec3 lookAt{0.0, 0.7, -3.2};
  Vec3 lookFrom;
  Vec3 vup;
  // elegir preset de camara sin modificar la escena
  auto toLower = [](std::string s){ for (auto& c : s) c = (char)tolower(c); return s; };
  std::string view = toLower(cameraView);
  double fovDeg = 45.0;
  if (view == "frontal" || view == "front") {
    lookFrom = Vec3{0.0, 1.0, 1.8};
    vup = Vec3{0.0, 1.0, 0.0};
  } else if (view == "superior" || view == "top") {
    // vista superior con un poquito de inclinacion para ver bien todos los objetos
    // para mantenernos abajo del techo seteo y < 2.5 y se abre mas el fov (75)
    lookFrom = Vec3{0.0, 2.45, 1.0};
    // inclinar mas hacia el piso bajando el punto de interes
    lookAt = Vec3{0.0, 0.30, -2.4};
    vup = Vec3{0.0, 1.0, 0.0};
    fovDeg = 75.0;
  } else if (view == "lateral" || view == "side") {
    // vista lateral desde la izquierda mirando hacia el centro
    // no podemos superar x=-2.0 (pared izquierda). Alejamos un poco y abrimos FOV.
    lookFrom = Vec3{-1.95, 1.0, -3.2};
    vup = Vec3{0.0, 1.0, 0.0};
    fovDeg = 70.0;
  } else {
    // por defecto, frontal
    lookFrom = Vec3{0.0, 1.0, 1.8};
    vup = Vec3{0.0, 1.0, 0.0};
  }
  double aspect = (double)width / (double)height;
  cam = new Camera(lookFrom, lookAt, vup, fovDeg, aspect);

  // fondo
  scene.background = Vec3{0.7, 0.8, 1.0};
}

int main(int argc, char** argv) {
  Args args = parseArgs(argc, argv);

  // crear escena segun seleccion
  Scene scene;

  Camera* cam = nullptr;
  if (args.scene == "base") {
    buildBaseScene(scene, cam, args.width, args.height, args.camera);
  } else {
    buildFinalScene(scene, cam, args.width, args.height, args.camera);
  }
  Renderer renderer(args.width, args.height, args.spp, args.maxDepth);
  auto pixels = renderer.render(scene, *cam, RenderMode::Final);
  bool ok = ImageWriterAuto::write(args.out, args.width, args.height, pixels, true);
  delete cam;
  if (!ok) {
    std::cerr << "error: no se pudo escribir la imagen en " << args.out << "\n";
    return 1;
  }
  std::cout << "listo: " << args.out << "\n";
  return 0;
}


