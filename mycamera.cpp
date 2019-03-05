#include "mycamera.h"

#include <QEvent>
#include <QHoverEvent>
#include <QKeyEvent>
#include <QWheelEvent>

#include <QDebug>

void MyCamera::doMovement()
{
    //camera controll
    if(m_keys[Qt::Key_W])
        Position+=MovementSpeed*Front;
    if(m_keys[Qt::Key_S])
        Position-=MovementSpeed*Front;
    if(m_keys[Qt::Key_A])
        Position -=glm::normalize(glm::cross(Front,Up))*MovementSpeed;
    if(m_keys[Qt::Key_D])
        Position +=glm::normalize(glm::cross(Front,Up))*MovementSpeed;
}

bool MyCamera::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->objectName()!="MyOpenglWindow")
        return false;

    if(event->type() == QEvent::KeyPress){
        QKeyEvent* key_evet = static_cast<QKeyEvent*>(event);
//        qDebug() << "KeyPressed:: "<<key_evet->key();
        m_keys[key_evet->key()] = true;
//        doMovement();
        if(key_evet->key()==Qt::Key_T){
           qDebug()<<"Camera Position:: "<<Position.x<<" "<<Position.y<<" "<<Position.z;
           qDebug()<<"Camera Yaw:: "<<Yaw;
           qDebug()<<"Camera Pitch:: "<<Pitch;

        }
        return true;
    }else if(event->type() == QEvent::KeyRelease){
        QKeyEvent* key_evet = static_cast<QKeyEvent*>(event);
//        qDebug()<<"isAutoRepeat"<<key_evet->isAutoRepeat();
        if(key_evet->isAutoRepeat() ) {
            return false;
        } else {
//            qDebug() << "keyReleaseEvent:: "<<key_evet->key() ;
            m_keys[key_evet->key()] = false;
//            doMovement();
            return true;
        }
    }else if(event->type() == QEvent::HoverEnter){
        lastX = (GLfloat)static_cast<QHoverEvent*>(event)->posF().x();
        lastY = (GLfloat)static_cast<QHoverEvent*>(event)->posF().y();
        updateCameraVectors();
        return true;
    }else if(event->type() == QEvent::HoverMove){
        GLfloat curPosX = (GLfloat)static_cast<QHoverEvent*>(event)->posF().x();
        GLfloat curPosY = (GLfloat)static_cast<QHoverEvent*>(event)->posF().y();

        GLfloat xoffset = curPosX - lastX;
        GLfloat yoffset = lastY - curPosY;
        lastX = curPosX;
        lastY = curPosY;

        GLfloat sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if(Pitch>89.0f)
            Pitch = 89.0f;
        if(Pitch<-89.0f)
            Pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw))*cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        updateCameraVectors();
        return true;
    }else if(event->type() == QEvent::Wheel){
        QWheelEvent* wheel_evet = static_cast<QWheelEvent*>(event);
        QPoint numSteps = wheel_evet->angleDelta()/120;
        double yoffset = numSteps.y();

        if(Zoom>=1.0f && Zoom <=45.0f)
            Zoom -= yoffset*0.1;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;

        updateCameraVectors();
        return true;
    }

}

void MyCamera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    this->Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
}
