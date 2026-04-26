## Physics

### Box2D-lite modified for embedded devices

Box2D Lite is a minimal, rigid-body physics engine originally written by Erin Catto as part of his GDC 2006 presentation. It implements sequential impulse-based constraint solving over a fixed timestep with contact caching, making it small enough and well-suited to heavy modification for embedded devices.

Dynamic memory allocation has been removed in favor of fixed-size ETL storage, floating-point types are narrowed to `float` to match the FPv5-D16 hard-float pipeline on the i.MX RT1062.
