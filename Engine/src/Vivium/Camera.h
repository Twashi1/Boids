#pragma once

#include "Core.h"
#include "Logger.h"
#include "Math.h"

// Lots ripped from here
// https://www.youtube.com/watch?v=NjKv-HWstxA&ab_channel=TheCherno

namespace Vivium {
    struct VIVIUM_API Camera {
    private:
        glm::mat4 m_ProjMat; // Projection matrix
        glm::mat4 m_ViewMat; // View matrix

        glm::vec3 m_Scale = glm::vec3(1.0f);    // Camera scale
        glm::vec3 m_Pos;                        // Camera pos
        float m_Rotation = 0.0f;                // Camera rotation

        void m_UpdateMatrix();

    public:
        void SetPos(const glm::vec3& pos);
        void SetPos(const Vector2<float>& pos);

        void SetCamera(const Vector2<float>& pos, const Vector2<float>& offset, float scale = 1.0f, float rotation = 0.0f);
        void SetCamera(const Vector2<float>& pos, const Vector2<float>& offset, const Vector2<float>& scale, float rotation = 0.0f);

        void SetScale(float scale);

        void SetRotation(float rotation);

        void SetProj(float left, float right, float bottom, float top);

        Vector2<float> Untransform(const Vector2<float>& point) const;
        Vector2<float> Transform(const Vector2<float>& point) const;

        const glm::vec3& GetPos() const;
        const glm::vec3& GetScale() const;

        float GetRotation() const;

        void SetProjMat(const glm::mat4& proj);
        const glm::mat4& GetProjMat() const;
        void SetViewMat(const glm::mat4& view);
        const glm::mat4& GetViewMat() const;

        Camera();
        Camera(float left, float right, float bottom, float top);
    };
}
