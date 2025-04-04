#include "assets.h"

#ifdef DEBUG
#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../third_party/stb_image.h"
#pragma warning(pop)
#endif

// 纹理文件路径
STATIC char *TEXTURE_PATHS[] = {"assets/textures/TEXTURE_ATLAS_01.png"};
static_assert(ARRAY_SIZE(TEXTURE_PATHS) == TEXTURE_COUNT);

// 获取资源数据
char *get_asset(TextureID textureID, int *width, int *height)
{
    char *data = 0;
#ifdef DEBUG
    int nrChannels;
    char *stbiData = (char *)stbi_load(TEXTURE_PATHS[textureID], width, height, &nrChannels, 4);
    if (stbiData)
    {
        data = platform_allocate_transient(4 * *width * *height);
        memcpy(data, stbiData, 4 * *width * *height);
        stbi_image_free(stbiData);
    }
#else
    ASSERT(0, "需要实现资源包文件");
#endif
    return data;
}

// 获取文件最后修改时间戳
long long get_last_edit_timestamp(TextureID textureID)
{
    return platform_last_edit_timestamp(TEXTURE_PATHS[textureID]);
}