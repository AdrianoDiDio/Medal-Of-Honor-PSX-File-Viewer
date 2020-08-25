/*
===========================================================================
    Copyright (C) 2018-2020 Adriano Di Dio.
    
    MOHLevelViewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MOHLevelViewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MOHLevelViewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/ 
#ifndef __GLLOADERCORE_H_
#define __GLLOADERCORE_H_
/**
 * Contains the core (3.3) functions definitions.
 */
#include <GL/gl.h>
#include <GL/glx.h>
extern void (APIENTRY *aglBlendFunc)(GLenum Sfactor, GLenum Dfactor);
extern void (APIENTRY *aglClear)(GLbitfield Mask);
extern void (APIENTRY *aglClearColor)(GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Alpha);
extern void (APIENTRY *aglClearDepth)(GLdouble Depth);
extern void (APIENTRY *aglClearStencil)(GLint s);
extern void (APIENTRY *aglColorMask)(GLboolean Red, GLboolean Green, GLboolean Blue, GLboolean Alpha);
extern void (APIENTRY *aglCullFace)(GLenum Mode);
extern void (APIENTRY *aglDepthFunc)(GLenum Func);
extern void (APIENTRY *aglDepthMask)(GLboolean Flag);
extern void (APIENTRY *aglDepthRange)(GLdouble Near, GLdouble Far);
extern void (APIENTRY *aglDisable)(GLenum Cap);
extern void (APIENTRY *aglDrawBuffer)(GLenum Mode);
extern void (APIENTRY *aglEnable)(GLenum Cap);
extern void (APIENTRY *aglFinish)();
extern void (APIENTRY *aglFlush)();
extern void (APIENTRY *aglFrontFace)(GLenum Mode);
extern void (APIENTRY *aglGetBooleanv)(GLenum Pname, GLboolean * Params);
extern void (APIENTRY *aglGetDoublev)(GLenum Pname, GLdouble * Params);
extern GLenum (APIENTRY *aglGetError)();
extern void (APIENTRY *aglGetFloatv)(GLenum Pname, GLfloat * Params);
extern void (APIENTRY *aglGetIntegerv)(GLenum Pname, GLint * Params);
extern const GLubyte * (APIENTRY *aglGetString)(GLenum Name);
extern void (APIENTRY *aglGetTexImage)(GLenum Target, GLint Level, GLenum Format, GLenum Type, GLvoid * Pixels);
extern void (APIENTRY *aglGetTexLevelParameterfv)(GLenum Target, GLint Level, GLenum Pname, GLfloat * Params);
extern void (APIENTRY *aglGetTexLevelParameteriv)(GLenum Target, GLint Level, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetTexParameterfv)(GLenum Target, GLenum Pname, GLfloat * Params);
extern void (APIENTRY *aglGetTexParameteriv)(GLenum Target, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglHint)(GLenum Target, GLenum Mode);
extern GLboolean (APIENTRY *aglIsEnabled)(GLenum Cap);
extern void (APIENTRY *aglLineWidth)(GLfloat Width);
extern void (APIENTRY *aglLogicOp)(GLenum Opcode);
extern void (APIENTRY *aglPixelStoref)(GLenum Pname, GLfloat Param);
extern void (APIENTRY *aglPixelStorei)(GLenum Pname, GLint Param);
extern void (APIENTRY *aglPointSize)(GLfloat Size);
extern void (APIENTRY *aglPolygonMode)(GLenum Face, GLenum Mode);
extern void (APIENTRY *aglReadBuffer)(GLenum Mode);
extern void (APIENTRY *aglReadPixels)(GLint x, GLint y, GLsizei Width, GLsizei Height, GLenum Format, GLenum Type, GLvoid * Pixels);
extern void (APIENTRY *aglScissor)(GLint x, GLint y, GLsizei Width, GLsizei Height);
extern void (APIENTRY *aglStencilFunc)(GLenum Func, GLint Ref, GLuint Mask);
extern void (APIENTRY *aglStencilMask)(GLuint Mask);
extern void (APIENTRY *aglStencilOp)(GLenum Fail, GLenum Zfail, GLenum Zpass);
extern void (APIENTRY *aglTexImage1D)(GLenum Target, GLint Level, GLint Internalformat, GLsizei Width, GLint Border, GLenum Format, GLenum Type, const GLvoid * Pixels);
extern void (APIENTRY *aglTexImage2D)(GLenum Target, GLint Level, GLint Internalformat, GLsizei Width, GLsizei Height, GLint Border, GLenum Format, GLenum Type, const GLvoid * Pixels);
extern void (APIENTRY *aglTexParameterf)(GLenum Target, GLenum Pname, GLfloat Param);
extern void (APIENTRY *aglTexParameterfv)(GLenum Target, GLenum Pname, const GLfloat * Params);
extern void (APIENTRY *aglTexParameteri)(GLenum Target, GLenum Pname, GLint Param);
extern void (APIENTRY *aglTexParameteriv)(GLenum Target, GLenum Pname, const GLint * Params);
extern void (APIENTRY *aglViewport)(GLint x, GLint y, GLsizei Width, GLsizei Height);

extern void (APIENTRY *aglBindTexture)(GLenum Target, GLuint Texture);
extern void (APIENTRY *aglCopyTexImage1D)(GLenum Target, GLint Level, GLenum Internalformat, GLint x, GLint y, GLsizei Width, GLint Border);
extern void (APIENTRY *aglCopyTexImage2D)(GLenum Target, GLint Level, GLenum Internalformat, GLint x, GLint y, GLsizei Width, GLsizei Height, GLint Border);
extern void (APIENTRY *aglCopyTexSubImage1D)(GLenum Target, GLint Level, GLint Xoffset, GLint x, GLint y, GLsizei Width);
extern void (APIENTRY *aglCopyTexSubImage2D)(GLenum Target, GLint Level, GLint Xoffset, GLint Yoffset, GLint x, GLint y, GLsizei Width, GLsizei Height);
extern void (APIENTRY *aglDeleteTextures)(GLsizei n, const GLuint * Textures);
extern void (APIENTRY *aglDrawArrays)(GLenum Mode, GLint First, GLsizei Count);
extern void (APIENTRY *aglDrawElements)(GLenum Mode, GLsizei Count, GLenum Type, const GLvoid * Indices);
extern void (APIENTRY *aglGenTextures)(GLsizei n, GLuint * Textures);
extern GLboolean (APIENTRY *aglIsTexture)(GLuint Texture);
extern void (APIENTRY *aglPolygonOffset)(GLfloat Factor, GLfloat Units);
extern void (APIENTRY *aglTexSubImage1D)(GLenum Target, GLint Level, GLint Xoffset, GLsizei Width, GLenum Format, GLenum Type, const GLvoid * Pixels);
extern void (APIENTRY *aglTexSubImage2D)(GLenum Target, GLint Level, GLint Xoffset, GLint Yoffset, GLsizei Width, GLsizei Height, GLenum Format, GLenum Type, const GLvoid * Pixels);

extern void (APIENTRY *aglBlendColor)(GLfloat Red, GLfloat Green, GLfloat Blue, GLfloat Alpha);
extern void (APIENTRY *aglBlendEquation)(GLenum Mode);
extern void (APIENTRY *aglCopyTexSubImage3D)(GLenum Target, GLint Level, GLint Xoffset, GLint Yoffset, GLint Zoffset, GLint x, GLint y, GLsizei Width, GLsizei Height);
extern void (APIENTRY *aglDrawRangeElements)(GLenum Mode, GLuint Start, GLuint End, GLsizei Count, GLenum Type, const GLvoid * Indices);
extern void (APIENTRY *aglTexImage3D)(GLenum Target, GLint Level, GLint Internalformat, GLsizei Width, GLsizei Height, GLsizei Depth, GLint Border, GLenum Format, GLenum Type, const GLvoid * Pixels);
extern void (APIENTRY *aglTexSubImage3D)(GLenum Target, GLint Level, GLint Xoffset, GLint Yoffset, GLint Zoffset, GLsizei Width, GLsizei Height, GLsizei Depth, GLenum Format, GLenum Type, const GLvoid * Pixels);

extern void (APIENTRY *aglActiveTexture)(GLenum Texture);
extern void (APIENTRY *aglCompressedTexImage1D)(GLenum Target, GLint Level, GLenum Internalformat, GLsizei Width, GLint Border, GLsizei ImageSize, const GLvoid * Data);
extern void (APIENTRY *aglCompressedTexImage2D)(GLenum Target, GLint Level, GLenum Internalformat, GLsizei Width, GLsizei Height, GLint Border, GLsizei ImageSize, const GLvoid * Data);
extern void (APIENTRY *aglCompressedTexImage3D)(GLenum Target, GLint Level, GLenum Internalformat, GLsizei Width, GLsizei Height, GLsizei Depth, GLint Border, GLsizei ImageSize, const GLvoid * Data);
extern void (APIENTRY *aglCompressedTexSubImage1D)(GLenum Target, GLint Level, GLint Xoffset, GLsizei Width, GLenum Format, GLsizei ImageSize, const GLvoid * Data);
extern void (APIENTRY *aglCompressedTexSubImage2D)(GLenum Target, GLint Level, GLint Xoffset, GLint Yoffset, GLsizei Width, GLsizei Height, GLenum Format, GLsizei ImageSize, const GLvoid * Data);
extern void (APIENTRY *aglCompressedTexSubImage3D)(GLenum Target, GLint Level, GLint Xoffset, GLint Yoffset, GLint Zoffset, GLsizei Width, GLsizei Height, GLsizei Depth, GLenum Format, GLsizei ImageSize, const GLvoid * Data);
extern void (APIENTRY *aglGetCompressedTexImage)(GLenum Target, GLint Level, GLvoid * Img);
extern void (APIENTRY *aglSampleCoverage)(GLfloat Value, GLboolean Invert);

extern void (APIENTRY *aglBlendFuncSeparate)(GLenum SfactorRGB, GLenum DfactorRGB, GLenum SfactorAlpha, GLenum DfactorAlpha);
extern void (APIENTRY *aglMultiDrawArrays)(GLenum Mode, const GLint * First, const GLsizei * Count, GLsizei Drawcount);
extern void (APIENTRY *aglMultiDrawElements)(GLenum Mode, const GLsizei * Count, GLenum Type, const GLvoid *const* Indices, GLsizei Drawcount);
extern void (APIENTRY *aglPointParameterf)(GLenum Pname, GLfloat Param);
extern void (APIENTRY *aglPointParameterfv)(GLenum Pname, const GLfloat * Params);
extern void (APIENTRY *aglPointParameteri)(GLenum Pname, GLint Param);
extern void (APIENTRY *aglPointParameteriv)(GLenum Pname, const GLint * Params);

extern void (APIENTRY *aglBeginQuery)(GLenum Target, GLuint Id);
extern void (APIENTRY *aglBindBuffer)(GLenum Target, GLuint Buffer);
extern void (APIENTRY *aglBufferData)(GLenum Target, GLsizeiptr Size, const GLvoid * Data, GLenum Usage);
extern void (APIENTRY *aglBufferSubData)(GLenum Target, GLintptr Offset, GLsizeiptr Size, const GLvoid * Data);
extern void (APIENTRY *aglDeleteBuffers)(GLsizei n, const GLuint * Buffers);
extern void (APIENTRY *aglDeleteQueries)(GLsizei n, const GLuint * Ids);
extern void (APIENTRY *aglEndQuery)(GLenum Target);
extern void (APIENTRY *aglGenBuffers)(GLsizei n, GLuint * Buffers);
extern void (APIENTRY *aglGenQueries)(GLsizei n, GLuint * Ids);
extern void (APIENTRY *aglGetBufferParameteriv)(GLenum Target, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetBufferPointerv)(GLenum Target, GLenum Pname, GLvoid ** Params);
extern void (APIENTRY *aglGetBufferSubData)(GLenum Target, GLintptr Offset, GLsizeiptr Size, GLvoid * Data);
extern void (APIENTRY *aglGetQueryObjectiv)(GLuint Id, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetQueryObjectuiv)(GLuint Id, GLenum Pname, GLuint * Params);
extern void (APIENTRY *aglGetQueryiv)(GLenum Target, GLenum Pname, GLint * Params);
extern GLboolean (APIENTRY *aglIsBuffer)(GLuint Buffer);
extern GLboolean (APIENTRY *aglIsQuery)(GLuint Id);
extern void * (APIENTRY *aglMapBuffer)(GLenum Target, GLenum Access);
extern GLboolean (APIENTRY *aglUnmapBuffer)(GLenum Target);

extern void (APIENTRY *aglAttachShader)(GLuint Program, GLuint Shader);
extern void (APIENTRY *aglBindAttribLocation)(GLuint Program, GLuint Index, const GLchar * Name);
extern void (APIENTRY *aglBlendEquationSeparate)(GLenum ModeRGB, GLenum ModeAlpha);
extern void (APIENTRY *aglCompileShader)(GLuint Shader);
extern GLuint (APIENTRY *aglCreateProgram)();
extern GLuint (APIENTRY *aglCreateShader)(GLenum Type);
extern void (APIENTRY *aglDeleteProgram)(GLuint Program);
extern void (APIENTRY *aglDeleteShader)(GLuint Shader);
extern void (APIENTRY *aglDetachShader)(GLuint Program, GLuint Shader);
extern void (APIENTRY *aglDisableVertexAttribArray)(GLuint Index);
extern void (APIENTRY *aglDrawBuffers)(GLsizei n, const GLenum * Bufs);
extern void (APIENTRY *aglEnableVertexAttribArray)(GLuint Index);
extern void (APIENTRY *aglGetActiveAttrib)(GLuint Program, GLuint Index, GLsizei BufSize, GLsizei * Length, GLint * Size, GLenum * Type, GLchar * Name);
extern void (APIENTRY *aglGetActiveUniform)(GLuint Program, GLuint Index, GLsizei BufSize, GLsizei * Length, GLint * Size, GLenum * Type, GLchar * Name);
extern void (APIENTRY *aglGetAttachedShaders)(GLuint Program, GLsizei MaxCount, GLsizei * Count, GLuint * Shaders);
extern GLint (APIENTRY *aglGetAttribLocation)(GLuint Program, const GLchar * Name);
extern void (APIENTRY *aglGetProgramInfoLog)(GLuint Program, GLsizei BufSize, GLsizei * Length, GLchar * InfoLog);
extern void (APIENTRY *aglGetProgramiv)(GLuint Program, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetShaderInfoLog)(GLuint Shader, GLsizei BufSize, GLsizei * Length, GLchar * InfoLog);
extern void (APIENTRY *aglGetShaderSource)(GLuint Shader, GLsizei BufSize, GLsizei * Length, GLchar * Source);
extern void (APIENTRY *aglGetShaderiv)(GLuint Shader, GLenum Pname, GLint * Params);
extern GLint (APIENTRY *aglGetUniformLocation)(GLuint Program, const GLchar * Name);
extern void (APIENTRY *aglGetUniformfv)(GLuint Program, GLint Location, GLfloat * Params);
extern void (APIENTRY *aglGetUniformiv)(GLuint Program, GLint Location, GLint * Params);
extern void (APIENTRY *aglGetVertexAttribPointerv)(GLuint Index, GLenum Pname, GLvoid ** Pointer);
extern void (APIENTRY *aglGetVertexAttribdv)(GLuint Index, GLenum Pname, GLdouble * Params);
extern void (APIENTRY *aglGetVertexAttribfv)(GLuint Index, GLenum Pname, GLfloat * Params);
extern void (APIENTRY *aglGetVertexAttribiv)(GLuint Index, GLenum Pname, GLint * Params);
extern GLboolean (APIENTRY *aglIsProgram)(GLuint Program);
extern GLboolean (APIENTRY *aglIsShader)(GLuint Shader);
extern void (APIENTRY *aglLinkProgram)(GLuint Program);
extern void (APIENTRY *aglShaderSource)(GLuint Shader, GLsizei Count, const GLchar *const* String, const GLint * Length);
extern void (APIENTRY *aglStencilFuncSeparate)(GLenum Face, GLenum Func, GLint Ref, GLuint Mask);
extern void (APIENTRY *aglStencilMaskSeparate)(GLenum Face, GLuint Mask);
extern void (APIENTRY *aglStencilOpSeparate)(GLenum Face, GLenum Sfail, GLenum Dpfail, GLenum Dppass);
extern void (APIENTRY *aglUniform1f)(GLint Location, GLfloat V0);
extern void (APIENTRY *aglUniform1fv)(GLint Location, GLsizei Count, const GLfloat * Value);
extern void (APIENTRY *aglUniform1i)(GLint Location, GLint V0);
extern void (APIENTRY *aglUniform1iv)(GLint Location, GLsizei Count, const GLint * Value);
extern void (APIENTRY *aglUniform2f)(GLint Location, GLfloat V0, GLfloat V1);
extern void (APIENTRY *aglUniform2fv)(GLint Location, GLsizei Count, const GLfloat * Value);
extern void (APIENTRY *aglUniform2i)(GLint Location, GLint V0, GLint V1);
extern void (APIENTRY *aglUniform2iv)(GLint Location, GLsizei Count, const GLint * Value);
extern void (APIENTRY *aglUniform3f)(GLint Location, GLfloat V0, GLfloat V1, GLfloat V2);
extern void (APIENTRY *aglUniform3fv)(GLint Location, GLsizei Count, const GLfloat * Value);
extern void (APIENTRY *aglUniform3i)(GLint Location, GLint V0, GLint V1, GLint V2);
extern void (APIENTRY *aglUniform3iv)(GLint Location, GLsizei Count, const GLint * Value);
extern void (APIENTRY *aglUniform4f)(GLint Location, GLfloat V0, GLfloat V1, GLfloat V2, GLfloat V3);
extern void (APIENTRY *aglUniform4fv)(GLint Location, GLsizei Count, const GLfloat * Value);
extern void (APIENTRY *aglUniform4i)(GLint Location, GLint V0, GLint V1, GLint V2, GLint V3);
extern void (APIENTRY *aglUniform4iv)(GLint Location, GLsizei Count, const GLint * Value);
extern void (APIENTRY *aglUniformMatrix2fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix3fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix4fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUseProgram)(GLuint Program);
extern void (APIENTRY *aglValidateProgram)(GLuint Program);
extern void (APIENTRY *aglVertexAttrib1d)(GLuint Index, GLdouble x);
extern void (APIENTRY *aglVertexAttrib1dv)(GLuint Index, const GLdouble * v);
extern void (APIENTRY *aglVertexAttrib1f)(GLuint Index, GLfloat x);
extern void (APIENTRY *aglVertexAttrib1fv)(GLuint Index, const GLfloat * v);
extern void (APIENTRY *aglVertexAttrib1s)(GLuint Index, GLshort x);
extern void (APIENTRY *aglVertexAttrib1sv)(GLuint Index, const GLshort * v);
extern void (APIENTRY *aglVertexAttrib2d)(GLuint Index, GLdouble x, GLdouble y);
extern void (APIENTRY *aglVertexAttrib2dv)(GLuint Index, const GLdouble * v);
extern void (APIENTRY *aglVertexAttrib2f)(GLuint Index, GLfloat x, GLfloat y);
extern void (APIENTRY *aglVertexAttrib2fv)(GLuint Index, const GLfloat * v);
extern void (APIENTRY *aglVertexAttrib2s)(GLuint Index, GLshort x, GLshort y);
extern void (APIENTRY *aglVertexAttrib2sv)(GLuint Index, const GLshort * v);
extern void (APIENTRY *aglVertexAttrib3d)(GLuint Index, GLdouble x, GLdouble y, GLdouble z);
extern void (APIENTRY *aglVertexAttrib3dv)(GLuint Index, const GLdouble * v);
extern void (APIENTRY *aglVertexAttrib3f)(GLuint Index, GLfloat x, GLfloat y, GLfloat z);
extern void (APIENTRY *aglVertexAttrib3fv)(GLuint Index, const GLfloat * v);
extern void (APIENTRY *aglVertexAttrib3s)(GLuint Index, GLshort x, GLshort y, GLshort z);
extern void (APIENTRY *aglVertexAttrib3sv)(GLuint Index, const GLshort * v);
extern void (APIENTRY *aglVertexAttrib4Nbv)(GLuint Index, const GLbyte * v);
extern void (APIENTRY *aglVertexAttrib4Niv)(GLuint Index, const GLint * v);
extern void (APIENTRY *aglVertexAttrib4Nsv)(GLuint Index, const GLshort * v);
extern void (APIENTRY *aglVertexAttrib4Nub)(GLuint Index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
extern void (APIENTRY *aglVertexAttrib4Nubv)(GLuint Index, const GLubyte * v);
extern void (APIENTRY *aglVertexAttrib4Nuiv)(GLuint Index, const GLuint * v);
extern void (APIENTRY *aglVertexAttrib4Nusv)(GLuint Index, const GLushort * v);
extern void (APIENTRY *aglVertexAttrib4bv)(GLuint Index, const GLbyte * v);
extern void (APIENTRY *aglVertexAttrib4d)(GLuint Index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
extern void (APIENTRY *aglVertexAttrib4dv)(GLuint Index, const GLdouble * v);
extern void (APIENTRY *aglVertexAttrib4f)(GLuint Index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
extern void (APIENTRY *aglVertexAttrib4fv)(GLuint Index, const GLfloat * v);
extern void (APIENTRY *aglVertexAttrib4iv)(GLuint Index, const GLint * v);
extern void (APIENTRY *aglVertexAttrib4s)(GLuint Index, GLshort x, GLshort y, GLshort z, GLshort w);
extern void (APIENTRY *aglVertexAttrib4sv)(GLuint Index, const GLshort * v);
extern void (APIENTRY *aglVertexAttrib4ubv)(GLuint Index, const GLubyte * v);
extern void (APIENTRY *aglVertexAttrib4uiv)(GLuint Index, const GLuint * v);
extern void (APIENTRY *aglVertexAttrib4usv)(GLuint Index, const GLushort * v);
extern void (APIENTRY *aglVertexAttribPointer)(GLuint Index, GLint Size, GLenum Type, GLboolean Normalized, GLsizei Stride, const GLvoid * Pointer);

extern void (APIENTRY *aglUniformMatrix2x3fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix2x4fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix3x2fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix3x4fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix4x2fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);
extern void (APIENTRY *aglUniformMatrix4x3fv)(GLint Location, GLsizei Count, GLboolean Transpose, const GLfloat * Value);

extern void (APIENTRY *aglBeginConditionalRender)(GLuint Id, GLenum Mode);
extern void (APIENTRY *aglBeginTransformFeedback)(GLenum PrimitiveMode);
extern void (APIENTRY *aglBindBufferBase)(GLenum Target, GLuint Index, GLuint Buffer);
extern void (APIENTRY *aglBindBufferRange)(GLenum Target, GLuint Index, GLuint Buffer, GLintptr Offset, GLsizeiptr Size);
extern void (APIENTRY *aglBindFragDataLocation)(GLuint Program, GLuint Color, const GLchar * Name);
extern void (APIENTRY *aglBindFramebuffer)(GLenum Target, GLuint Framebuffer);
extern void (APIENTRY *aglBindRenderbuffer)(GLenum Target, GLuint Renderbuffer);
extern void (APIENTRY *aglBindVertexArray)(GLuint Array);
extern void (APIENTRY *aglBlitFramebuffer)(GLint SrcX0, GLint SrcY0, GLint SrcX1, GLint SrcY1, GLint DstX0, GLint DstY0, GLint DstX1, GLint DstY1, GLbitfield Mask, GLenum Filter);
extern GLenum (APIENTRY *aglCheckFramebufferStatus)(GLenum Target);
extern void (APIENTRY *aglClampColor)(GLenum Target, GLenum Clamp);
extern void (APIENTRY *aglClearBufferfi)(GLenum Buffer, GLint Drawbuffer, GLfloat Depth, GLint Stencil);
extern void (APIENTRY *aglClearBufferfv)(GLenum Buffer, GLint Drawbuffer, const GLfloat * Value);
extern void (APIENTRY *aglClearBufferiv)(GLenum Buffer, GLint Drawbuffer, const GLint * Value);
extern void (APIENTRY *aglClearBufferuiv)(GLenum Buffer, GLint Drawbuffer, const GLuint * Value);
extern void (APIENTRY *aglColorMaski)(GLuint Index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
extern void (APIENTRY *aglDeleteFramebuffers)(GLsizei n, const GLuint * Framebuffers);
extern void (APIENTRY *aglDeleteRenderbuffers)(GLsizei n, const GLuint * Renderbuffers);
extern void (APIENTRY *aglDeleteVertexArrays)(GLsizei n, const GLuint * Arrays);
extern void (APIENTRY *aglDisablei)(GLenum Target, GLuint Index);
extern void (APIENTRY *aglEnablei)(GLenum Target, GLuint Index);
extern void (APIENTRY *aglEndConditionalRender)();
extern void (APIENTRY *aglEndTransformFeedback)();
extern void (APIENTRY *aglFlushMappedBufferRange)(GLenum Target, GLintptr Offset, GLsizeiptr Length);
extern void (APIENTRY *aglFramebufferRenderbuffer)(GLenum Target, GLenum Attachment, GLenum Renderbuffertarget, GLuint Renderbuffer);
extern void (APIENTRY *aglFramebufferTexture1D)(GLenum Target, GLenum Attachment, GLenum Textarget, GLuint Texture, GLint Level);
extern void (APIENTRY *aglFramebufferTexture2D)(GLenum Target, GLenum Attachment, GLenum Textarget, GLuint Texture, GLint Level);
extern void (APIENTRY *aglFramebufferTexture3D)(GLenum Target, GLenum Attachment, GLenum Textarget, GLuint Texture, GLint Level, GLint Zoffset);
extern void (APIENTRY *aglFramebufferTextureLayer)(GLenum Target, GLenum Attachment, GLuint Texture, GLint Level, GLint Layer);
extern void (APIENTRY *aglGenFramebuffers)(GLsizei n, GLuint * Framebuffers);
extern void (APIENTRY *aglGenRenderbuffers)(GLsizei n, GLuint * Renderbuffers);
extern void (APIENTRY *aglGenVertexArrays)(GLsizei n, GLuint * Arrays);
extern void (APIENTRY *aglGenerateMipmap)(GLenum Target);
extern void (APIENTRY *aglGetBooleani_v)(GLenum Target, GLuint Index, GLboolean * Data);
extern GLint (APIENTRY *aglGetFragDataLocation)(GLuint Program, const GLchar * Name);
extern void (APIENTRY *aglGetFramebufferAttachmentParameteriv)(GLenum Target, GLenum Attachment, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetIntegeri_v)(GLenum Target, GLuint Index, GLint * Data);
extern void (APIENTRY *aglGetRenderbufferParameteriv)(GLenum Target, GLenum Pname, GLint * Params);
extern const GLubyte * (APIENTRY *aglGetStringi)(GLenum Name, GLuint Index);
extern void (APIENTRY *aglGetTexParameterIiv)(GLenum Target, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetTexParameterIuiv)(GLenum Target, GLenum Pname, GLuint * Params);
extern void (APIENTRY *aglGetTransformFeedbackVarying)(GLuint Program, GLuint Index, GLsizei BufSize, GLsizei * Length, GLsizei * Size, GLenum * Type, GLchar * Name);
extern void (APIENTRY *aglGetUniformuiv)(GLuint Program, GLint Location, GLuint * Params);
extern void (APIENTRY *aglGetVertexAttribIiv)(GLuint Index, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetVertexAttribIuiv)(GLuint Index, GLenum Pname, GLuint * Params);
extern GLboolean (APIENTRY *aglIsEnabledi)(GLenum Target, GLuint Index);
extern GLboolean (APIENTRY *aglIsFramebuffer)(GLuint Framebuffer);
extern GLboolean (APIENTRY *aglIsRenderbuffer)(GLuint Renderbuffer);
extern GLboolean (APIENTRY *aglIsVertexArray)(GLuint Array);
extern void * (APIENTRY *aglMapBufferRange)(GLenum Target, GLintptr Offset, GLsizeiptr Length, GLbitfield Access);
extern void (APIENTRY *aglRenderbufferStorage)(GLenum Target, GLenum Internalformat, GLsizei Width, GLsizei Height);
extern void (APIENTRY *aglRenderbufferStorageMultisample)(GLenum Target, GLsizei Samples, GLenum Internalformat, GLsizei Width, GLsizei Height);
extern void (APIENTRY *aglTexParameterIiv)(GLenum Target, GLenum Pname, const GLint * Params);
extern void (APIENTRY *aglTexParameterIuiv)(GLenum Target, GLenum Pname, const GLuint * Params);
extern void (APIENTRY *aglTransformFeedbackVaryings)(GLuint Program, GLsizei Count, const GLchar *const* Varyings, GLenum BufferMode);
extern void (APIENTRY *aglUniform1ui)(GLint Location, GLuint V0);
extern void (APIENTRY *aglUniform1uiv)(GLint Location, GLsizei Count, const GLuint * Value);
extern void (APIENTRY *aglUniform2ui)(GLint Location, GLuint V0, GLuint V1);
extern void (APIENTRY *aglUniform2uiv)(GLint Location, GLsizei Count, const GLuint * Value);
extern void (APIENTRY *aglUniform3ui)(GLint Location, GLuint V0, GLuint V1, GLuint V2);
extern void (APIENTRY *aglUniform3uiv)(GLint Location, GLsizei Count, const GLuint * Value);
extern void (APIENTRY *aglUniform4ui)(GLint Location, GLuint V0, GLuint V1, GLuint V2, GLuint V3);
extern void (APIENTRY *aglUniform4uiv)(GLint Location, GLsizei Count, const GLuint * Value);
extern void (APIENTRY *aglVertexAttribI1i)(GLuint Index, GLint x);
extern void (APIENTRY *aglVertexAttribI1iv)(GLuint Index, const GLint * v);
extern void (APIENTRY *aglVertexAttribI1ui)(GLuint Index, GLuint x);
extern void (APIENTRY *aglVertexAttribI1uiv)(GLuint Index, const GLuint * v);
extern void (APIENTRY *aglVertexAttribI2i)(GLuint Index, GLint x, GLint y);
extern void (APIENTRY *aglVertexAttribI2iv)(GLuint Index, const GLint * v);
extern void (APIENTRY *aglVertexAttribI2ui)(GLuint Index, GLuint x, GLuint y);
extern void (APIENTRY *aglVertexAttribI2uiv)(GLuint Index, const GLuint * v);
extern void (APIENTRY *aglVertexAttribI3i)(GLuint Index, GLint x, GLint y, GLint z);
extern void (APIENTRY *aglVertexAttribI3iv)(GLuint Index, const GLint * v);
extern void (APIENTRY *aglVertexAttribI3ui)(GLuint Index, GLuint x, GLuint y, GLuint z);
extern void (APIENTRY *aglVertexAttribI3uiv)(GLuint Index, const GLuint * v);
extern void (APIENTRY *aglVertexAttribI4bv)(GLuint Index, const GLbyte * v);
extern void (APIENTRY *aglVertexAttribI4i)(GLuint Index, GLint x, GLint y, GLint z, GLint w);
extern void (APIENTRY *aglVertexAttribI4iv)(GLuint Index, const GLint * v);
extern void (APIENTRY *aglVertexAttribI4sv)(GLuint Index, const GLshort * v);
extern void (APIENTRY *aglVertexAttribI4ubv)(GLuint Index, const GLubyte * v);
extern void (APIENTRY *aglVertexAttribI4ui)(GLuint Index, GLuint x, GLuint y, GLuint z, GLuint w);
extern void (APIENTRY *aglVertexAttribI4uiv)(GLuint Index, const GLuint * v);
extern void (APIENTRY *aglVertexAttribI4usv)(GLuint Index, const GLushort * v);
extern void (APIENTRY *aglVertexAttribIPointer)(GLuint Index, GLint Size, GLenum Type, GLsizei Stride, const GLvoid * Pointer);

extern void (APIENTRY *aglCopyBufferSubData)(GLenum ReadTarget, GLenum WriteTarget, GLintptr ReadOffset, GLintptr WriteOffset, GLsizeiptr Size);
extern void (APIENTRY *aglDrawArraysInstanced)(GLenum Mode, GLint First, GLsizei Count, GLsizei Instancecount);
extern void (APIENTRY *aglDrawElementsInstanced)(GLenum Mode, GLsizei Count, GLenum Type, const GLvoid * Indices, GLsizei Instancecount);
extern void (APIENTRY *aglGetActiveUniformBlockName)(GLuint Program, GLuint UniformBlockIndex, GLsizei BufSize, GLsizei * Length, GLchar * UniformBlockName);
extern void (APIENTRY *aglGetActiveUniformBlockiv)(GLuint Program, GLuint UniformBlockIndex, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetActiveUniformName)(GLuint Program, GLuint UniformIndex, GLsizei BufSize, GLsizei * Length, GLchar * UniformName);
extern void (APIENTRY *aglGetActiveUniformsiv)(GLuint Program, GLsizei UniformCount, const GLuint * UniformIndices, GLenum Pname, GLint * Params);
extern GLuint (APIENTRY *aglGetUniformBlockIndex)(GLuint Program, const GLchar * UniformBlockName);
extern void (APIENTRY *aglGetUniformIndices)(GLuint Program, GLsizei UniformCount, const GLchar *const* UniformNames, GLuint * UniformIndices);
extern void (APIENTRY *aglPrimitiveRestartIndex)(GLuint Index);
extern void (APIENTRY *aglTexBuffer)(GLenum Target, GLenum Internalformat, GLuint Buffer);
extern void (APIENTRY *aglUniformBlockBinding)(GLuint Program, GLuint UniformBlockIndex, GLuint UniformBlockBinding);

extern GLenum (APIENTRY *aglClientWaitSync)(GLsync Sync, GLbitfield Flags, GLuint64 Timeout);
extern void (APIENTRY *aglDeleteSync)(GLsync Sync);
extern void (APIENTRY *aglDrawElementsBaseVertex)(GLenum Mode, GLsizei Count, GLenum Type, const GLvoid * Indices, GLint Basevertex);
extern void (APIENTRY *aglDrawElementsInstancedBaseVertex)(GLenum Mode, GLsizei Count, GLenum Type, const GLvoid * Indices, GLsizei Instancecount, GLint Basevertex);
extern void (APIENTRY *aglDrawRangeElementsBaseVertex)(GLenum Mode, GLuint Start, GLuint End, GLsizei Count, GLenum Type, const GLvoid * Indices, GLint Basevertex);
extern GLsync (APIENTRY *aglFenceSync)(GLenum Condition, GLbitfield Flags);
extern void (APIENTRY *aglFramebufferTexture)(GLenum Target, GLenum Attachment, GLuint Texture, GLint Level);
extern void (APIENTRY *aglGetBufferParameteri64v)(GLenum Target, GLenum Pname, GLint64 * Params);
extern void (APIENTRY *aglGetInteger64i_v)(GLenum Target, GLuint Index, GLint64 * Data);
extern void (APIENTRY *aglGetInteger64v)(GLenum Pname, GLint64 * Params);
extern void (APIENTRY *aglGetMultisamplefv)(GLenum Pname, GLuint Index, GLfloat * Val);
extern void (APIENTRY *aglGetSynciv)(GLsync Sync, GLenum Pname, GLsizei BufSize, GLsizei * Length, GLint * Values);
extern GLboolean (APIENTRY *aglIsSync)(GLsync Sync);
extern void (APIENTRY *aglMultiDrawElementsBaseVertex)(GLenum Mode, const GLsizei * Count, GLenum Type, const GLvoid *const* Indices, GLsizei Drawcount, const GLint * Basevertex);
extern void (APIENTRY *aglProvokingVertex)(GLenum Mode);
extern void (APIENTRY *aglSampleMaski)(GLuint Index, GLbitfield Mask);
extern void (APIENTRY *aglTexImage2DMultisample)(GLenum Target, GLsizei Samples, GLint Internalformat, GLsizei Width, GLsizei Height, GLboolean Fixedsamplelocations);
extern void (APIENTRY *aglTexImage3DMultisample)(GLenum Target, GLsizei Samples, GLint Internalformat, GLsizei Width, GLsizei Height, GLsizei Depth, GLboolean Fixedsamplelocations);
extern void (APIENTRY *aglWaitSync)(GLsync Sync, GLbitfield Flags, GLuint64 Timeout);

extern void (APIENTRY *aglBindFragDataLocationIndexed)(GLuint Program, GLuint ColorNumber, GLuint Index, const GLchar * Name);
extern void (APIENTRY *aglBindSampler)(GLuint Unit, GLuint Sampler);
extern void (APIENTRY *aglDeleteSamplers)(GLsizei Count, const GLuint * Samplers);
extern void (APIENTRY *aglGenSamplers)(GLsizei Count, GLuint * Samplers);
extern GLint (APIENTRY *aglGetFragDataIndex)(GLuint Program, const GLchar * Name);
extern void (APIENTRY *aglGetQueryObjecti64v)(GLuint Id, GLenum Pname, GLint64 * Params);
extern void (APIENTRY *aglGetQueryObjectui64v)(GLuint Id, GLenum Pname, GLuint64 * Params);
extern void (APIENTRY *aglGetSamplerParameterIiv)(GLuint Sampler, GLenum Pname, GLint * Params);
extern void (APIENTRY *aglGetSamplerParameterIuiv)(GLuint Sampler, GLenum Pname, GLuint * Params);
extern void (APIENTRY *aglGetSamplerParameterfv)(GLuint Sampler, GLenum Pname, GLfloat * Params);
extern void (APIENTRY *aglGetSamplerParameteriv)(GLuint Sampler, GLenum Pname, GLint * Params);
extern GLboolean (APIENTRY *aglIsSampler)(GLuint Sampler);
extern void (APIENTRY *aglQueryCounter)(GLuint Id, GLenum Target);
extern void (APIENTRY *aglSamplerParameterIiv)(GLuint Sampler, GLenum Pname, const GLint * Param);
extern void (APIENTRY *aglSamplerParameterIuiv)(GLuint Sampler, GLenum Pname, const GLuint * Param);
extern void (APIENTRY *aglSamplerParameterf)(GLuint Sampler, GLenum Pname, GLfloat Param);
extern void (APIENTRY *aglSamplerParameterfv)(GLuint Sampler, GLenum Pname, const GLfloat * Param);
extern void (APIENTRY *aglSamplerParameteri)(GLuint Sampler, GLenum Pname, GLint Param);
extern void (APIENTRY *aglSamplerParameteriv)(GLuint Sampler, GLenum Pname, const GLint * Param);
extern void (APIENTRY *aglVertexAttribDivisor)(GLuint Index, GLuint Divisor);
extern void (APIENTRY *aglVertexAttribP1ui)(GLuint Index, GLenum Type, GLboolean Normalized, GLuint Value);
extern void (APIENTRY *aglVertexAttribP1uiv)(GLuint Index, GLenum Type, GLboolean Normalized, const GLuint * Value);
extern void (APIENTRY *aglVertexAttribP2ui)(GLuint Index, GLenum Type, GLboolean Normalized, GLuint Value);
extern void (APIENTRY *aglVertexAttribP2uiv)(GLuint Index, GLenum Type, GLboolean Normalized, const GLuint * Value);
extern void (APIENTRY *aglVertexAttribP3ui)(GLuint Index, GLenum Type, GLboolean Normalized, GLuint Value);
extern void (APIENTRY *aglVertexAttribP3uiv)(GLuint Index, GLenum Type, GLboolean Normalized, const GLuint * Value);
extern void (APIENTRY *aglVertexAttribP4ui)(GLuint Index, GLenum Type, GLboolean Normalized, GLuint Value);
extern void (APIENTRY *aglVertexAttribP4uiv)(GLuint Index, GLenum Type, GLboolean Normalized, const GLuint * Value);

bool GL_InitCoreProfile(const char *Driver);

#endif //__GLLOADERCORE_H_
