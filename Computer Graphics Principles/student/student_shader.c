/*!
 * @file
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <math.h>

#include <student/gpu.h>
#include <student/student_shader.h>
#include <student/uniforms.h>

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(GPUVertexShaderOutput *const      output,
                        GPUVertexShaderInput const *const input,
                        GPU const                         gpu) {
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici
  /// čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních
  /// atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret*
  /// definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce
  /// shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()

  // get handle of all uniform variables
  Uniforms const handleUniformVariables = gpu_getUniformsHandle(gpu);
  // get uniform location of viewMatrix and projectionMatrix
  UniformLocation locationViewMatrix = getUniformLocation(gpu, "viewMatrix");
  UniformLocation locationProjectionMatrix = getUniformLocation(gpu, "projectionMatrix");
  // get pointer to both obtained matrix in the last steps
  Mat4 const*const viewMatrix = shader_interpretUniformAsMat4(handleUniformVariables, locationViewMatrix);
  Mat4 const*const projectionMatrix = shader_interpretUniformAsMat4(handleUniformVariables, locationProjectionMatrix);
  // interpret attribute of vertex and normal of its input as Vec3 type
  Vec3 const*const vertexAttributeIn = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
  Vec3 const*const normalAttributeIn = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);
  // lorem ipsum
  Mat4 multiplyMatrix;
  multiply_Mat4_Mat4(&multiplyMatrix, projectionMatrix, viewMatrix);
  // lorem ipsum 
  Vec4 copyVector;
  copy_Vec3Float_To_Vec4(&copyVector, vertexAttributeIn, 1.0f);
  // multiply matrix4 with vector4
  multiply_Mat4_Vec4(&output->gl_Position, &multiplyMatrix, &copyVector);
  // lorem ipsum 
  Vec3 *const vertexAttributeOut = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
  Vec3 *const normalAttributeOut = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);
  // lorem ipsum
  init_Vec3(vertexAttributeOut, vertexAttributeIn->data[0], vertexAttributeIn->data[1], vertexAttributeIn->data[2]);
  init_Vec3(normalAttributeOut, normalAttributeIn->data[0], normalAttributeIn->data[1], normalAttributeIn->data[2]);
}

void phong_fragmentShader(GPUFragmentShaderOutput *const      output,
                          GPUFragmentShaderInput const *const input,
                          GPU const                           gpu) {
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici
  /// světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte podle normály povrchu.
  /// V případě, že normála směřuje kolmo vzhůru je difuzní barva čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je difuzní barva čiště zelená.
  /// Difuzní barvu spočtěte lineární interpolací zelené a bíle barvy pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()

  // definition all needed Vectors to this work
	Vec3 interpolationNormal, lightDirection, diffuseView, viewDirection, reflectDirection, resultSpecular, summary;
  Vec3 spectacularColor = {{1, 1, 1}};
  // get handle of all uniform variables
  Uniforms const handleUniformVariables = gpu_getUniformsHandle(gpu);
  // interpret first and second attibute of input fragment of fragment shader
	Vec3 const *fragmentLocation = fs_interpretInputAttributeAsVec3(gpu, input, 0);
	normalize_Vec3(&interpolationNormal, fs_interpretInputAttributeAsVec3(gpu, input, 1));
	sub_Vec3(&lightDirection, shader_interpretUniformAsVec3(handleUniformVariables, getUniformLocation(gpu, "lightPosition")), fragmentLocation);
	normalize_Vec3(&lightDirection, &lightDirection);
  // product of dot from the vectors representing the Normal and Light
	float interpolationLight = dot_Vec3(&interpolationNormal, &lightDirection);
  // if the result negative must be zero, if is greather than zero must be one and else stay the result
  interpolationLight = (interpolationLight < 0) ? 0 : ((interpolationLight > 0 && interpolationLight < 0) ? 1 : interpolationLight);
  // multiplication both elements of interpolations normal
	float multiInterpolation = interpolationNormal.data[1] * interpolationNormal.data[1];
  multiInterpolation = (multiInterpolation < 0) ? 0 : ((multiInterpolation > 0 && multiInterpolation < 0) ? 1 : multiInterpolation);
	Vec3 diffuseColor = {{multiInterpolation, 1, multiInterpolation}};
	multiply_Vec3_Float(&diffuseView, &diffuseColor, interpolationLight);
	sub_Vec3 (&viewDirection, shader_interpretUniformAsVec3(handleUniformVariables, getUniformLocation(gpu,"cameraPosition")), fragmentLocation);
	normalize_Vec3(&viewDirection, &viewDirection);
	multiply_Vec3_Float(&lightDirection, &lightDirection, -1);
	reflect(&reflectDirection,&lightDirection, &interpolationNormal);
  // product of dot from the vectors representing the view and reflect directions
	float reflectionView = dot_Vec3(&viewDirection, &reflectDirection);
  // if the result negative must be zero, if is greather than zero must be one and else stay the result
  reflectionView = (reflectionView < 0) ? 0 : ((reflectionView > 0 && reflectionView < 0) ? 1 : reflectionView);
  // Shinesses effect to the power function such as exponent
	multiply_Vec3_Float(&resultSpecular, &spectacularColor, (float) pow(reflectionView, 40));
	add_Vec3(&summary, &resultSpecular, &diffuseView);
	copy_Vec3Float_To_Vec4(&output->color, &summary, 1);
}

/// @}
