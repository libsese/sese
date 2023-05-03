# Dockerfile

dev.dockerfile 构建用于当前项目的便捷开发环境镜像

使用如下命令构建镜像

```sh
cd docker && docker build -f dev.dockerfile -t sese-dev-image .
```

使用该镜像创建容器

```sh
docker run -itd sese-dev-image
```