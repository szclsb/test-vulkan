#include "ove_camera.h"

namespace ove {
    void OveCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        projectionMatrix = glm::mat4{1.0f};
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (bottom - top);
        projectionMatrix[2][2] = 1.f / (far - near);
        projectionMatrix[3][0] = -(right + left) / (right - left);
        projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
        projectionMatrix[3][2] = -near / (far - near);
    }

    void OveCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(fovy / 2.f);
        projectionMatrix = glm::mat4{0.0f};
        projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
        projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        projectionMatrix[2][2] = far / (far - near);
        projectionMatrix[2][3] = 1.f;
        projectionMatrix[3][2] = -(far * near) / (far - near);
    }

    void OveCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
        const glm::vec3 w{glm::normalize(direction)};
        const glm::vec3 u{glm::normalize(glm::cross(w, up))};
        const glm::vec3 v{glm::cross(w, u)};

        transformationMatrix = glm::mat4{1.f};
        transformationMatrix[0][0] = u.x;
        transformationMatrix[1][0] = u.y;
        transformationMatrix[2][0] = u.z;
        transformationMatrix[0][1] = v.x;
        transformationMatrix[1][1] = v.y;
        transformationMatrix[2][1] = v.z;
        transformationMatrix[0][2] = w.x;
        transformationMatrix[1][2] = w.y;
        transformationMatrix[2][2] = w.z;
        transformationMatrix[3][0] = -glm::dot(u, position);
        transformationMatrix[3][1] = -glm::dot(v, position);
        transformationMatrix[3][2] = -glm::dot(w, position);
    }

    void OveCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
        setViewDirection(position, target - position, up);
    }

    void OveCamera::setViewXyz(glm::vec3 position, glm::vec3 rotation) {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        transformationMatrix = glm::mat4{1.f};
        transformationMatrix[0][0] = u.x;
        transformationMatrix[1][0] = u.y;
        transformationMatrix[2][0] = u.z;
        transformationMatrix[0][1] = v.x;
        transformationMatrix[1][1] = v.y;
        transformationMatrix[2][1] = v.z;
        transformationMatrix[0][2] = w.x;
        transformationMatrix[1][2] = w.y;
        transformationMatrix[2][2] = w.z;
        transformationMatrix[3][0] = -glm::dot(u, position);
        transformationMatrix[3][1] = -glm::dot(v, position);
        transformationMatrix[3][2] = -glm::dot(w, position);
    }
}


