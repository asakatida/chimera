import pathlib

import jinja2
import yaml

supported_distros_matrix = {
    distro["name"]: dict(
        distro,
        tags={
            tag["name"]: dict(
                tag, images={image["name"]: image for image in tag["images"]}
            )
            for tag in distro["tags"]
        },
    )
    for distro in yaml.safe_load(
        pathlib.Path("tools/supported_distros.yml").read_text()
    )["distros"]
}


def key(item):
    def level(name):
        image = supported_distros_matrix[item["distro"]]["tags"][item["tag"]]["images"][
            name
        ]
        if "requires" in image:
            return 1 + level(image["requires"])
        return 1

    return (level(item["image"]), item["image"], item["distro"], item["tag"])


supported_distros = sorted(
    (
        {
            "dependencies": " ".join(
                dependency["name"] for dependency in image["dependencies"]
            ),
            "distro": distro["name"],
            "environment": image["environment"],
            "image": image["name"],
            "package_command": distro["package_command"],
            "requires": image.get("requires"),
            "tag": tag["name"],
        }
        for distro in supported_distros_matrix.values()
        for tag in distro["tags"].values()
        for image in tag["images"].values()
    ),
    key=key,
)
min_supported_distros = [
    {"distro": distro["distro"], "image": distro["image"], "tag": distro["tag"]}
    for distro in supported_distros
]

jinja2.Template(
    pathlib.Path("tools/docker.yml.j2").read_text(),
    autoescape=True,
    block_end_string="%>",
    block_start_string="<%",
    comment_end_string="#>",
    comment_start_string="<#",
    enable_async=True,
    keep_trailing_newline=False,
    lstrip_blocks=True,
    optimized=True,
    trim_blocks=True,
    variable_end_string=">>",
    variable_start_string="<<",
).stream(
    supported_distros=supported_distros, min_supported_distros=min_supported_distros
).dump(
    "tools/docker.yml"
)
