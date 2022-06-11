/*
**  Vulkan backend
**  Copyright (c) 2016-2020 Magnus Norddahl
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
*/

#include "vk_ppshader.h"
#include "vulkan/system/vk_framebuffer.h"
#include "vulkan/system/vk_builders.h"
#include "filesystem.h"

VkPPShader::VkPPShader(VulkanFrameBuffer* fb, PPShader *shader)
{
	FString prolog;
	if (!shader->Uniforms.empty())
		prolog = UniformBlockDecl::Create("Uniforms", shader->Uniforms, -1);
	prolog += shader->Defines;

	ShaderBuilder vertbuilder;
	vertbuilder.setVertexShader(LoadShaderCode(shader->VertexShader, "", shader->Version));
	VertexShader = vertbuilder.create(shader->VertexShader.GetChars(), fb->device);
	VertexShader->SetDebugName(shader->VertexShader.GetChars());

	ShaderBuilder fragbuilder;
	fragbuilder.setFragmentShader(LoadShaderCode(shader->FragmentShader, prolog, shader->Version));
	FragmentShader = fragbuilder.create(shader->FragmentShader.GetChars(), fb->device);
	FragmentShader->SetDebugName(shader->FragmentShader.GetChars());
}

FString VkPPShader::LoadShaderCode(const FString &lumpName, const FString &defines, int version)
{
	int lump = fileSystem.CheckNumForFullName(lumpName);
	if (lump == -1) I_FatalError("Unable to load '%s'", lumpName.GetChars());
	FString code = fileSystem.ReadFile(lump).GetString().GetChars();

	FString patchedCode;
	patchedCode.AppendFormat("#version %d\n", 450);
	patchedCode << defines;
	patchedCode << "#line 1\n";
	patchedCode << code;
	return patchedCode;
}