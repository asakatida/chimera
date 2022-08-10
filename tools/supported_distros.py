import pathlib

import jinja2
import yaml

supported_distros_matrix = yaml.safe_load(
    pathlib.Path("tools/supported_distros.yml").read_text()
)


def key(item: dict[str, str]) -> tuple[int, str, str, str]:
    def level(name: str) -> int:
        image = supported_distros_matrix[item["distro"]]["tags"][item["tag"]][name]
        if "requires" in image:
            return 1 + level(image["requires"])
        return 1

    return (level(item["image"]), item["image"], item["distro"], item["tag"])


supported_distros = sorted(
    (
        {
            "dependencies": " ".join(image["dependencies"]),
            "distro": distro_name,
            "environment": image["environment"],
            "image": image_name,
            "package_command": distro["package_command"],
            "requires": image.get("requires"),
            "tag": tag_name,
        }
        for distro_name, distro in supported_distros_matrix.items()
        for tag_name, tag in distro["tags"].items()
        for image_name, image in tag.items()
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
