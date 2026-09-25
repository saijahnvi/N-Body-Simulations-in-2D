# 2D N-Body Gravity Simulator

An interactive real-time physics simulation engine built in **C++20** using **Modern OpenGL (3.3 Core Profile)**, **GLFW**, **GLEW**, and **GLM**. It models pairwise mutual gravitational interaction between multiple celestial bodies in 2D space while handling real-time elastic collision response and positional overlap correction.

---

## Visual Demonstrations

https://github.com/user-attachments/assets/c0ace057-0dbe-41cc-b1de-39f511e47875

https://github.com/user-attachments/assets/e92984c7-3ed2-4124-8a9a-9fa43f5bacdb

https://github.com/user-attachments/assets/e5b28d46-e47a-43ad-a86f-38a0f0543565

Different configurations of the n bodies are provided in the text file with their respective velocities and positions.
---

## Technical Highlights & Architecture

### 1. Gravitational Mechanics
The engine computes pairwise mutual gravitational attraction across all active bodies ($O(N^2)$ algorithmic complexity per frame). Acceleration vectors are calculated by applying Newton's Law of Universal Gravitation combined with 2D unit vector normalization:

$$\vec{a}_i = G \sum_{j \neq i} \frac{m_j}{\vert{}\vec{r}_{ij}\vert{}^2} \hat{r}_{ij}$$

* **Distance Softening:** A lower bound cutoff ($r_{\text{min}} = 0.05$) prevents numerical singularity blow-ups (infinite force) during high-velocity close encounters.

### 2. Impulse Collision & Overlap Resolution
Collisions between spherical bodies are resolved in two distinct steps:
1. **Positional Correction:** Prevents objects from sticking together due to numerical integration drift by separating overlapping geometry proportionally to inverse mass ratios:
   $$\Delta \vec{p}_A = \frac{m_B}{m_A + m_B} (\text{overlap}) \cdot \hat{n}$$
2. **Elastic Impulse Response:** Calculates 1D linear momentum transfer along the collision normal with a coefficient of restitution $e = 1.0$:
   $$j = \frac{-(1 + e) (\vec{v}_{\text{rel}} \cdot \hat{n})}{\frac{1}{m_A} + \frac{1}{m_B}}$$

### 3. Modern OpenGL Rendering Pipeline
* Renders procedurally generated triangle-fan circle primitives managed via custom GLSL Vertex and Fragment shaders.
* Updates entity transform uniforms (`u_Offset`) per object while streaming vertex attribute pointers through dynamic Vertex Buffer Objects (`GL_DYNAMIC_DRAW`) bound to a Vertex Array Object (VAO).
* Utilizes frame delta-time integration ($\Delta t$) for framerate-independent particle propagation.

---

## Project Structure

```text
├── main.cpp                # Core application entry point, simulation loops, and shader pipeline
├── dependencies/
│   ├── include/            # GLM, GLFW, and GLEW header files
│   └── library/            # Pre-compiled static and dynamic libraries (.dylib)
├── Makefile                # Target compilation rules and linker flags
└── README.md
