#include "evenbettercap.h"

using namespace Monopticon::Level3;


Address::Address(UnsignedByte id, Object3D& object, Figure::PhongIdShader& shader, Color3 &color, GL::Mesh& mesh, SceneGraph::DrawableGroup3D& group):
    Object3D{&object},
    SceneGraph::Drawable3D{object, &group},
    _id{id},
    _color{color},
    _shader{shader},
    _mesh{mesh}
{
    setCachedTransformations(SceneGraph::CachedTransformation::Absolute);
    setClean();
}


void Address::clean(const Matrix4& absoluteTransformation) {
    _absolutePosition = absoluteTransformation.translation();
}


void Address::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
    auto tm = transformationMatrix;

    // TODO investigate why _cubemesh normals are not producing expected specularity
    _shader.setTransformationMatrix(tm)
           .setNormalMatrix(tm.rotationScaling())
           .setProjectionMatrix(camera.projectionMatrix())
           .setAmbientColor(_color*0.2)
           .setTimeIntensity(0.9)
           .setColor(_color*0.9)
           /* relative to the camera */
           .setLightPosition({0.0f, 4.0f, 3.0f})
           .setObjectId(_id);

    _mesh.draw(_shader);
}


Vector3 Address::getTranslation() {
    // NOTE could call setClean here to resolve any future movement calls
    return _absolutePosition;
}
