#include "Leaf.h"

// Function to generate a random double value between two values
float fRand(float fMin, float fMax)
{
    float f = (float)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


Leaf::Leaf(): oldU(0.0f), oldV(0.0001f), oldOmega(0.0f), oldAlpha(0.0f)         //Note: v cannot be zero!!
{
    oldX = fRand(-7.0f, 7.0f);      // these should be randoms
	oldY = fRand(-5.0f, 10.0f);
    z    = fRand(-5.0f, 5.0f);

    //oldAlpha = fRand(-1.0f, 1.0f);
    oldTheta = fRand(-1.0f, 1.1f);
    rotZ = fRand(M_PI/2-1, M_PI/2);

	//Values needed for rotation
    kort = 5; //fRand(1.0f, 10.0f);    // Ortogonal friction
    kpar = kort/50;     // Parallel friction
    length = fRand(0.5f,0.8f);
    //length = fRand(0.3f,0.5f);

    //Set wind:
    windX = 10;
    windY = 4;

    mesh.createBox(length, length, 0.00001f);
}

void Leaf::update(float h, GLFWwindow *window)
{
    float condition = oldAlpha+oldTheta;    // This is used to handle whether t should be + or -
    float V2 = oldU*oldU + oldV*oldV; // The magnitude of the speed of the leaf ^2

    //Check condition
    if(oldAlpha+oldTheta < M_PI || oldAlpha+oldTheta > M_PI)
        condition = (float)fmod(oldAlpha+oldTheta, M_PI);

    if(oldV < 0 && condition > 0 && condition < M_PI || oldV > 0 && condition > -M_PI && condition < 0)
    {
        u = oldU + (-(kort*sin(oldTheta)*sin(oldTheta) + kpar*cos(oldTheta)*cos(oldTheta))*oldU
            + (kort - kpar)*sin(oldTheta)*cos(oldTheta)*oldV
            - M_PI*RHO*(V2)*cos(oldTheta + oldTheta)*cos(oldTheta))*h;

        v = oldV + ((kort - kpar)*sin(oldTheta)*cos(oldTheta)*oldU
            - (kort*cos(oldTheta)*cos(oldTheta) + kpar*sin(oldTheta)*sin(oldTheta))*oldV
            + M_PI*RHO*(V2)*cos(oldAlpha + oldTheta)*sin(oldAlpha) - g)*h;
    }
    else if(oldV < 0  && condition > -M_PI && condition < 0  || oldV > 0 && condition > 0 && condition < M_PI )
    {
        u = oldU + (-(kort*sin(oldTheta)*sin(oldTheta) + kpar*cos(oldTheta)*cos(oldTheta))*oldU
            + (kort - kpar)*sin(oldTheta)*cos(oldTheta)*oldV
            + M_PI*RHO*(V2)*cos(oldTheta + oldTheta)*cos(oldTheta))*h;

        v = oldV + ((kort - kpar)*sin(oldTheta)*cos(oldTheta)*oldU
            - (kort*cos(oldTheta)*cos(oldTheta) + kpar*sin(oldTheta)*sin(oldTheta))*oldV
            - M_PI*RHO*(V2)*cos(oldAlpha + oldTheta)*sin(oldAlpha) - g)*h;
    }

    alpha = atan(u/v); // New movement direction

    omega = oldOmega +
           (-kort*oldOmega - (3*M_PI*RHO*(V2)/length)*cos(oldAlpha + oldTheta)*sin(oldAlpha + oldTheta))*h;

    theta = oldTheta + oldOmega*h;

    // The position becomes:
    //int UpArrow = 0;
    //UpArrow = glfwGetKey(GLFW_KEY_UP);

    if(glfwGetKey(window, GLFW_KEY_RIGHT)){ //Bl�s �t h�ger
        x = oldX + (oldU+windX)*h; //Alla hastigheter �r nu relativa. Vind l�ggs p� i efterhand.
    }
    else if(glfwGetKey(window, GLFW_KEY_LEFT)){ //Bl�s �t v�nster
        x = oldX + (oldU-windX)*h;
    }
    else{
        x = oldX + (oldU)*h;
    }

    if(glfwGetKey(window, GLFW_KEY_UP)){ //Bl�s upp
        y = oldY + (oldV+windY)*h;
    }
    else if(glfwGetKey(window, GLFW_KEY_DOWN)){ //Bl�s ner
        y = oldY + (oldV-windY)*h;
    }
    else{

        y = oldY + (oldV)*h;
    }
    //If the leaf moves too far down, push it up again.
    if(y < -10.0)
    {
        x = fRand(-20.0f, 10.0f);      // these should be randoms
        y = fRand(6.0f, 7.0f);
    }

    // Update variables for nest iteration
    oldU = u;
    oldV = v;
    oldX = x;
    oldY = y;
    oldAlpha = alpha;
    oldOmega = omega;
    oldTheta = theta;

}

void Leaf::draw(MatrixStack& mStack, GLint& location_MV, float time)
{
    mStack.push(); //Save the current matrix before performing multiplications


        mStack.rotX(0.4);
        mStack.translate(x, y, z);
        //mStack.rotY(std::min(0.0f, (float)sin(time)));
        mStack.rotZ(theta);
        mStack.rotZ(rotZ);
        mStack.rotX(theta*0.8);

        glUniformMatrix4fv( location_MV, 1, GL_FALSE, mStack.getCurrentMatrix() );
        mesh.render(); //Draw the player

    mStack.pop(); //Restore the initial matrix
}

