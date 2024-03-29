# SCOP
SCOP is 42 project.Basic GPU rendering with OpenGL.

The program should load a 3D object from an .obj file and display it in a window using perspective projection.

The object should rotate around its central axis.

# How to Run
```
brew install glew
brew install glfw
```
And then
```
git clone https://github.com/shima8823/SCOP.git
cd SCOP
make
./scop <objects/42.obj> [textures/cat.bmp | Optional]
```

# Features
```
→ key, ← key                   　　: Move along the x and z axes
Shift + Up, Shift + Down       　　: Move along the y-axis
Cursor Movement                 : Change the viewpoint
Shift + P                       : Look at the origin
Shift + X, Shift + Y, Shift + Z : Change the rotation axis
Shift + T                       : Apply the texture specified by command-line arguments to the object
Shift + L                       : Set the light position to either the camera position or vec(0, 100, 100)
Shift + F                       : Fever mode!!
```

# Beautiful 
![scop_42](https://github.com/shima8823/SCOP/assets/57759015/c2c9d82f-1a3e-4c60-9e5b-c1116f37d9e5) ![scop_barrel](https://github.com/shima8823/SCOP/assets/57759015/b31ddf91-3476-4570-be9e-f85a5b1a3dd6)
![scop_teapot](https://github.com/shima8823/SCOP/assets/57759015/69b69f24-65e8-42cd-a11b-c559e55ce422)



https://github.com/shima8823/SCOP/assets/57759015/1f96a97c-2f72-4836-82c8-0984db852313

