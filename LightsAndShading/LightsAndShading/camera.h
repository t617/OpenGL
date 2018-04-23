
#ifndef CAMERA_H
#define CAMERA_H

#include gladglad.h
#include glmglm.hpp
#include glmgtcmatrix_transform.hpp

#include vector

 Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

 Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


 An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public
     Camera Attributes
    glmvec3 Position;
    glmvec3 Front;
    glmvec3 Up;
    glmvec3 Right;
    glmvec3 WorldUp;
     Euler Angles
    float Yaw;
    float Pitch;
     Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

     Constructor with vectors
    Camera(glmvec3 position = glmvec3(0.0f, 0.0f, 0.0f), glmvec3 up = glmvec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)  Front(glmvec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
     Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)  Front(glmvec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glmvec3(posX, posY, posZ);
        WorldUp = glmvec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

     Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glmmat4 GetViewMatrix()
    {
        return glmlookAt(Position, Position + Front, Up);
    }

     Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed  deltaTime;
        if (direction == FORWARD)
            Position += Front  velocity;
        if (direction == BACKWARD)
            Position -= Front  velocity;
        if (direction == LEFT)
            Position -= Right  velocity;
        if (direction == RIGHT)
            Position += Right  velocity;
    }

     Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset = MouseSensitivity;
        yoffset = MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

         Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch  89.0f)
                Pitch = 89.0f;
            if (Pitch  -89.0f)
                Pitch = -89.0f;
        }

         Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

     Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom = 1.0f && Zoom = 45.0f)
            Zoom -= yoffset;
        if (Zoom = 1.0f)
            Zoom = 1.0f;
        if (Zoom = 45.0f)
            Zoom = 45.0f;
    }

private
     Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
         Calculate the new Front vector
        glmvec3 front;
        front.x = cos(glmradians(Yaw))  cos(glmradians(Pitch));
        front.y = sin(glmradians(Pitch));
        front.z = sin(glmradians(Yaw))  cos(glmradians(Pitch));
        Front = glmnormalize(front);
         Also re-calculate the Right and Up vector
        Right = glmnormalize(glmcross(Front, WorldUp));   Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glmnormalize(glmcross(Right, Front));
    }
};
#endif