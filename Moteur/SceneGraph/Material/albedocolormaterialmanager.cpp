#include "albedocolormaterialmanager.h"
#include "../../Descriptor/descriptorpoolbuilder.h"
#include "../../Descriptor/descriptorsetlayoutbuilder.h"

AlbedoColorMaterialManager::AlbedoColorMaterialManager(vk::Device device, BufferFactory & bufferFactory) :
	AbstractUniqueMaterialManager(device),
	mBufferFactory(bufferFactory),
	mStagingBuffer(bufferFactory.createEmptyBuffer(sizeof(Descriptor), vk::BufferUsageFlagBits::eTransferSrc, false)),
	mBuffer(bufferFactory.createEmptyBuffer(sizeof(Descriptor) * 1000, vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, true))
{
	mDescriptorPool = std::make_unique<DescriptorPoolWrapper>(DescriptorPoolBuilder::monoDynamicUniformBuffer(device, 1));
	mDescriptorSetLayout = std::make_unique<DescriptorSetLayout>(DescriptorSetLayoutBuilder::monoUniformBufferDynamic(device, vk::ShaderStageFlagBits::eFragment));
	mDescriptorSet = mDescriptorPool->allocate(*mDescriptorSetLayout);

	vk::DescriptorBufferInfo ubo(mBuffer, 0, VK_WHOLE_SIZE);
	auto writeUbo = StructHelper::writeDescriptorSet(mDescriptorSet, 0, &ubo, vk::DescriptorType::eUniformBufferDynamic);
	mDevice.updateDescriptorSets({ writeUbo }, {});
}

bool AlbedoColorMaterialManager::isAccepted(Material material) const
{
	return material.isOnlyAlbedoColor();
}

AbstractUniqueMaterialManager::MaterialPointer AlbedoColorMaterialManager::addMaterial(Material material)
{
	MaterialPointer p;

	p.ptr = this;
	p.index = mValues.size();

	Descriptor descriptor;

	descriptor.color = glm::vec4(material.albedoColor, 1.0);

	mStagingBuffer.resetOffset();
	mStagingBuffer.push_data(descriptor);
	mBufferFactory.transfer(mStagingBuffer, mBuffer, 0, p.index * sizeof(Descriptor), sizeof(Descriptor));

	mValues.emplace_back(descriptor);

	return p;
}

void AlbedoColorMaterialManager::getDrawerMaterialValues(Drawer & drawer, const MaterialPointer & ptr) const
{
	AbstractUniqueMaterialManager::getDrawerMaterialValues(drawer, ptr);
	drawer.materialSet = mDescriptorSet;
	drawer.offsetMaterialUbo = ptr.index * sizeof(Descriptor);
}
