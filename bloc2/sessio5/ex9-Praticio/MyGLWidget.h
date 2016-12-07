#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include"model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();
    void modelTransformRot ();
    void projectTransform();
    void viewTransform();
    void init_camera();
    void initVariables();
    void paintModel(const Model &model, const GLuint &VAO);
    void paintTerra(const GLuint &VAO);
    void resizeAngle();
    void calcCapsaContenidora(const Model &model,glm::vec3& max,glm::vec3& min);
    float calculRadi( const glm::vec3& max, const glm::vec3& min);
    glm::vec3 calcCentreCapsa(const glm::vec3 &max, const glm::vec3 &min);
    float calculDistancia(glm::vec3 ini, glm::vec3 fi);

    //DECLARACIO UNIC OBJECTE
    Model m; // un únic model
    Model vectorModels[3]; // array de 3 models
    //vector<Model> models; // vector stl de models


    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc;
    //projection
    GLuint projLoc;
    //View
    GLuint viewLoc;
    // VAO i VBO names homer
    GLuint VAO_Homer, VBO_HomerPos, VBO_HomerCol;
    // VAO i VBO terra
    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraCol;
    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float FOV, ra, znear, zfar, angle,radi;
    float scale,rotate;
    glm::vec3 pos;
    glm::vec3 maxCapsa,minCapsa,centreCapsa;
    glm::vec3 OBS, VRP, UP;
};

