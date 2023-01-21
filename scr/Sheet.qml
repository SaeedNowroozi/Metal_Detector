import QtQuick
import QtQuick3D
Node {
    id: sheet_obj
    Model {
        id: cube
        source: "meshes/cube.mesh"

        DefaultMaterial {
            id: material_material
            diffuseColor: "#ffcccccc"
        }
        materials: [
            material_material
        ]
    }
}
