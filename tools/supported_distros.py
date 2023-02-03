from pathlib import Path

from yaml import dump, safe_load

supported_distros_matrix = safe_load(Path("tools/supported_distros.yml").read_text())


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

group_vars = Path("tools/group_vars/all")
group_vars.mkdir(parents=True, exist_ok=True)

with (group_vars / "supported_distros.yml").open("w") as ostream:
    dump(
        {
            "supported_distros": supported_distros,
            "min_supported_distros": min_supported_distros,
        },
        ostream,
    )
