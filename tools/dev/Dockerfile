#
#	MetaCall Library by Parra Studios
#	Docker image infrastructure for MetaCall.
#
#	Copyright (C) 2016 - 2019 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#	you may not use this file except in compliance with the License.
#	You may obtain a copy of the License at
#
#		http://www.apache.org/licenses/LICENSE-2.0
#
#	Unless required by applicable law or agreed to in writing, software
#	distributed under the License is distributed on an "AS IS" BASIS,
#	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#	See the License for the specific language governing permissions and
#	limitations under the License.
#

# MetaCall Depends node builder image
FROM metacall/core_deps_node AS builder_node

# Image descriptor
LABEL copyright.name="Vicente Eduardo Ferrer Garcia" \
	copyright.address="vic798@gmail.com" \
	maintainer.name="Vicente Eduardo Ferrer Garcia" \
	maintainer.address="vic798@gmail.com" \
	vendor="MetaCall Inc." \
	version="0.1"

# Arguments
ARG METACALL_PATH

# Environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Define working directory
WORKDIR $METACALL_PATH

# MetaCall Depends image
FROM metacall/core_deps AS builder

# Image descriptor
LABEL copyright.name="Vicente Eduardo Ferrer Garcia" \
	copyright.address="vic798@gmail.com" \
	maintainer.name="Vicente Eduardo Ferrer Garcia" \
	maintainer.address="vic798@gmail.com" \
	vendor="MetaCall Inc." \
	version="0.1"

# Arguments
ARG METACALL_PATH

# Environment variables
ENV LOADER_LIBRARY_PATH=$METACALL_PATH/build \
	LOADER_SCRIPT_PATH=$METACALL_PATH/build/scripts \
	CONFIGURATION_PATH=$METACALL_PATH/build/configurations/global.json \
	SERIAL_LIBRARY_PATH=$METACALL_PATH/build \
	DETOUR_LIBRARY_PATH=$METACALL_PATH/build \
	DEBIAN_FRONTEND=noninteractive

# Define working directory
WORKDIR $METACALL_PATH

# Copy MetaCall NodeJS dependencies
COPY --from=builder_node /usr/local/lib/libnode.so.* /usr/local/lib/

# Copy MetaCall dependecies
COPY . $METACALL_PATH

# Configure MetaCall build tool script
RUN chmod 500 $METACALL_PATH/tools/metacall-configure.sh \
	&& chmod 500 $METACALL_PATH/tools/metacall-build.sh \
	&& chmod 500 $METACALL_PATH/tools/metacall-clear.sh \
	&& mkdir -p $METACALL_PATH/build

# Configure MetaCall libraries and build runtimes if needed
ARG METACALL_BUILD_TYPE
ARG METACALL_BUILD_OPTIONS

RUN cd $METACALL_PATH/build \
	&& $METACALL_PATH/tools/metacall-configure.sh ${METACALL_BUILD_TYPE} ${METACALL_BUILD_OPTIONS}

# Build and install MetaCall libraries then run tests
RUN cd $METACALL_PATH/build \
	&& $METACALL_PATH/tools/metacall-build.sh ${METACALL_BUILD_TYPE} ${METACALL_BUILD_OPTIONS}

# Clear MetaCall build dependencies
RUN cd $METACALL_PATH/build \
	&& $METACALL_PATH/tools/metacall-clear.sh ${METACALL_BUILD_TYPE} ${METACALL_BUILD_OPTIONS}
