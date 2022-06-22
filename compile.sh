repo_path=$(git rev-parse --show-toplevel)

# build image
# docker build -t gcc-build ${repo_path}

# run container
docker run -v ${repo_path}:/app -it --rm --name=compile gcc-build
