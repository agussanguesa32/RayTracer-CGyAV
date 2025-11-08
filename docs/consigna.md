# COMP. GRÁFICA Y AMB. VIRTUALES (0494)
# SEGUNDO PARCIAL AÑO 2025
03/11/2025

---

## Objetivo

Implementar un trazador de rayos recursivo que permita simular iluminación global básica mediante reflexión, refracción y sombras. El propósito es demostrar comprensión de los fundamentos del trazado de rayos recursivo y de los modelos de iluminación, así como la capacidad de análisis y justificación de resultados visuales.

---

## Directivas

### Requisitos mínimos

Los elementos y características funcionales mínimas que debe contener el trazador de rayos son los siguientes:

1. Generación de rayos primarios desde la cámara.

2. Intersección con objetos simples: esfera y triángulo.

3. Iluminación local (ej. modelo Phong) usando al menos una luz puntual.

4. Iluminación global mediante rayos recursivos:

   a) Sombras

   b) Reflexión

   c) Refracción

5. Control de profundidad recursiva y color de fondo.

6. Al menos tres materiales distintos: difuso, especular y dieléctrico.

### Limitaciones

Sólo se admite el uso de alguna librería matemática (por ejemplo, GLM); las funcionalidades que forman el trazador de rayos y los modelos de iluminación deben ser implementaciones propias.


---

## Escena

Se podrá elegir entre dos opciones:

### A. Escena libre

Diseñar una escena con una composición libre siempre que cumpla los requisitos mínimos del trazador de rayos recursivo. La misma debe incluir al menos:

1. Tres objetos con materiales distintos (difuso, especular, dieléctrico)

2. Un plano o fondo visible

### B. Escena base

Una caja o plano de fondo y tres esferas con materiales diferentes:

1. Esfera difusa (lambertiana)

2. Esfera especular (reflexión tipo espejo)

3. Esfera dieléctrica (refracción tipo vidrio o burbuja)