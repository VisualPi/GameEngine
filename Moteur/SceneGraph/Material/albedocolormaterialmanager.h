#pragma once

#include "abstractuniquematerialmanager.h"

class AlbedoColorMaterialManager : public AbstractUniqueMaterialManager {
public:
	struct Descriptor {
		glm::vec4 color;
		glm::vec4 padding[15];
	};

	static_assert(sizeof(Descriptor) % 256 == 0, "Albedo color must be 256 bytes aligned");

	AlbedoColorMaterialManager(vk::Device device, BufferFactory &bufferFactory);

	bool isAccepted(Material material) const override;
	MaterialPointer addMaterial(Material material) override;
	void getDrawerMaterialValues(Drawer &drawer, const MaterialPointer &ptr) const override;

private:
	BufferFactory &mBufferFactory;
	Buffer mStagingBuffer;
	Buffer mBuffer;
	vk::DescriptorSet mDescriptorSet;
	std::vector<Descriptor> mValues;
};

