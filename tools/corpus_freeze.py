"""corpus_freeze.py"""

from asyncio import run
from base64 import b64encode
from json import dump, load
from lzma import compress
from os import environ
from pathlib import Path
from subprocess import PIPE
from sys import argv

from asyncio_as_completed import as_completed
from asyncio_cmd import cmd, main
from corpus_ascii import is_ascii
from corpus_utils import c_tqdm, corpus_objects

BAD_KEYS = {
    "00139b023f0791bc6273e11fec8e2b46848fde2f",
    "04df8bdbf14b8ea62f24fa13e70d2f8ca716ef26",
    "088c5a685f1ee48dc52ac629e48b9975b84f588c",
    "0a108b02eeeb963c96ee698a997cc56fbed13edb",
    "0d9692817b85ba09ee351f9287b4cea24b5a7f39",
    "1068e251e846f3af36d3c0243d6fdf0438e94979",
    "1a7c3c4089a70001635accd83e4e704138218235",
    "1b718fd6dcce6c80137f102dfbbb96adc357bb25",
    "1bfbaa6ce0c46625e6e9199a4b8acaf6034558da",
    "1c5f59b37dd87130525711282a6679c84d11ba7d",
    "1d4fa4d9b67969835a5be4adba588642bbcab8b3",
    "1da5676d5a2c2bc7afdb1bc0725b8fe09d9b0e7f",
    "1e445d9da094d2116c048d181b9d6247f497da6e",
    "1ea3f2207c7c8f41cf27b4864506c02ec79b4d24",
    "20c2b23417ffcccce6f6620cddf7d9aafab0e076",
    "21e8bb7646e5f103a2e9268c1dad25a85abc958e",
    "22ca0005f6b02d9a836ca283f185388e74953c65",
    "27fc6434e423c873f88ed3d3bd5d28e7c9836a6e",
    "2945c6fbf47f5e153f2a6366c67da660cd80d991",
    "2b8c78bffa170220cb375e12c514da0f22facbff",
    "2ba86f588202f2c8b42a367608bd42f8b90163a6",
    "2be24065ccf5784799712ad95fc683806e62badc",
    "2ef60831cf9d4773c230b2813b45510b5140b8ba",
    "3445b56c222fb54c6b0aaef7ae5a192ab6c9cb55",
    "3465bcd2d1d4985880d2c4605bdb28d1861e2170",
    "36284a218380f2cf00779ee0c0f32ce4900bf396",
    "3a814cc440c7f2db306065b94a6213620e3ebe6b",
    "3baebce0c291ac8989c4461a5b646418ac6bd148",
    "3caf83e361bb32ff036433c5ba733e06ff34a798",
    "3da645f7d20cb3284d70408de849ac8886446c0e",
    "42816e2348346cf2d2272c0e1d9973c8fcdb894f",
    "42f39a29f7d18163abe16da10fd51d70091c2fa8",
    "456afa5f8cd1347aa7ffc4b526c7517a6b8fbc6b",
    "4763681342557351a0312fa645836235bcbe85e0",
    "484ddd87814b153222794fe81eccb6b1a229c26d",
    "4b51e22e7aac165da290f2ff0d5edd2f4243007a",
    "4c81fbee72e05ccfae3d047ae5f1d9c5c85bfe3d",
    "4c8b5364cfa29a8a932c60061863c327aea88cce",
    "4f636b20e507500c6c53ecec09394b008dd07fc8",
    "501b2f3bbc8a57c52997f45e6ebe31d24e5a6007",
    "5107f7715b008af0457902fb74bb1402bd869bba",
    "514f4c42b62188812a76275ac02b91d736c535ed",
    "522e230e5faf8c38373405bf70a3ad73511ecefb",
    "55883d77efc6edfcfad364a36e69b08a798f3165",
    "559ab4fd0694332cc33a4c182e008d9b834cf3f3",
    "56698b0690a5c854fe59679d938f43c87226cc65",
    "578b3721a4abc1ba22bd875b86b79ada2358f20b",
    "57dbba4088f74f545e05a68099d161a0b9d097e6",
    "5875a1f00d22807731af954945ff86bd828b8c64",
    "59a2511bb2417a144234ee91cb6354e516d1225e",
    "5ea178c1d492f036687d3ca5e66a4892a4dc86c6",
    "6318d20c5f8ed3783f7c54bd2e566359b2b1dcef",
    "664e06c5cac1e75ac63ce5daca696309cfcb946c",
    "68dd57c21cb3d67b395e317f3d51ac155ba8f0da",
    "6f4137bdf068001881a685096e71a7703a55069c",
    "70567610104314b7bc38e068a11305b9f275cf3c",
    "74179c622eb99aabe6ee1acbce56238e5f01b040",
    "75a0cd33bdb74c586fe17e5663716ad30d33bf2d",
    "7a3af6902ce484db71f9efdc44c1a8bc53b33ebe",
    "7b1adbcee4063f8a19fcabb4e407f1b83667bfe9",
    "7da54a85b6f231137cdd8f61431bbb36f3f593ff",
    "80cb31effafeb4d5adbf9df666b473a3fe0ab170",
    "864c85a1aff96a1940db8942c5e7e921d2363c3d",
    "8d0c81108005d1e0a9495790861d8c659ac51a3d",
    "8e3155f7ce078c84be9463228f502d535f21871b",
    "909fffef34fd9db4dc5a9c1f66b33826f0436fd9",
    "916d5957c8f977879abd67367404df48bd5a10ad",
    "91c9dc646b35157e51a2a2c65e57817f1b1f9584",
    "929ff11f3aa5bc98b19b6858b0945d25ab59a6c8",
    "972e3a52e7f095f6648e315c62f666c51901a8d0",
    "985ee2133c90d4e6d974fa47ddf5fd8721bbfce2",
    "9929b5fdaa9e9d9ebba7abce429c9cd59ab365b8",
    "998848d18e7bc4e9ffe2f99874a0142d61aa8e0c",
    "9e576a25691fa16417d0ff729022650bc224435f",
    "a12e353f89b5067e636711064f450df744846d5e",
    "a23ea7021107ed9e49d90c6fcb136291d8c73914",
    "a4ce3a58cd59a09f8090a0cb1d6eb64125b40d17",
    "ab72dd4e378e5f7f8b58df485b40a84da21121df",
    "acfee37855edf1c374516b615cfeeb1e8a6260d3",
    "acffe2ab313f24421ebaf2a0e1317798fa511f9f",
    "af7fa358bc47be3966feee502f2c62a39856711f",
    "b11a5765dcd23411b176ecb1908649e0698f29ff",
    "b123a70b514d194b7435181abe6632db8b75f050",
    "b4345fb967cffbf09413779c0090ddf56c1c7364",
    "b578de17745a5f0a5c18235f8f54a3a28436942d",
    "b63eabb56a5ac943c29d0ff631f360c4c4c874d4",
    "b7a7d3648bb36ece1385a8eee2f675e8cd4786bb",
    "bbc48920cfa0952a92ec65b829f986d6ca7d7a81",
    "bdfe3af34ef5b04fe5cee3950f3bfc4f4003a22a",
    "c4108b3914c5b4ea532169f0c739d7e93b3171cf",
    "c4a9447bef48904bf5f6652dd6702e23f69a4275",
    "c8a22fdd932b8bf393a51b7def69d1b87bb7da89",
    "c8ef8f7bb2ba9c7c59595f698ed96366ac3c1e41",
    "cb1a33ea8865f506e53806af386c5de8e6589907",
    "cb4eeb9de7549ec673a2637d260ea1cd0ebf24a8",
    "cb765eea226d3e636ad1f925143321d8bdfe7fc4",
    "d20f32af5e154cdc0f6ad21f8dd5ea5949bc781c",
    "d5e18ac0f04e9bc26d4107e0596686905f6711e4",
    "d6adbf067cf542805290c59af6bc204ecec6ac12",
    "d6b8903dde20c3edbe18f05062fadefd3453b28c",
    "d8314a8e49fd591c1e7e45eb8388e7306743f0ce",
    "de2e4a649d4b6745c4a7cb9d4e42d2d3acff5faf",
    "e0c71a2ea37a952deec56378c066f5814bf66124",
    "e85f588bab00c34b64bfb51a0bd12d3dde2d8357",
    "e8c5bc873a0870d0213b8a87ced562968b33c184",
    "e9896ea0c89eab28efa9a8b18b852d901c34d30a",
    "ecd529b8159d71363838439a9d9bb5baa736e4a3",
    "efdefcd17bbb603e60f39f72d356793c1aac6b6b",
    "f20adff428459d7f3d4dc886ddd7ab5ac79f238b",
    "fb664b3a5373c56f5e48515e8b78dab061bfc93e",
    "fd37ab180df504531539224c029858aedac66d5a",
    "feeb32cc51d9a47522d9b0fecea2c43cd1098b37",
}


async def corpus_freeze(
    output: str,
    *,
    base_reference: str = environ.get("BASE_REF", "^origin/stable"),
    disable_bars: bool | None,
) -> None:
    file = Path(output)
    with file.open() as istream:
        cases = {key: value for key, value in load(istream).items()}
    crashes = [
        path for path in Path("unit_tests/fuzz/crashes").rglob("*") if path.is_file()
    ]
    existing = {
        key.strip().decode()
        for key in await as_completed(
            c_tqdm(
                (
                    cmd("git", "hash-object", path, log=False, out=PIPE)
                    for path in crashes
                ),
                "Hash corpus",
                disable_bars,
                total=len(crashes),
            )
        )
    }
    for key in BAD_KEYS.union(existing).intersection(cases.keys()):
        del cases[key]
    cases.update(
        (sha, b64encode(compress(obj)).decode())
        for sha, obj in await corpus_objects(
            "unit_tests/fuzz/corpus",
            "unit_tests/fuzz/crashes",
            base_reference=base_reference,
            disable_bars=disable_bars,
            exclude=BAD_KEYS.union(existing).union(cases.keys()),
        )
        if is_ascii(obj)
    )
    with file.open("w") as ostream:
        dump(cases, ostream, indent=4, sort_keys=True)


async def corpus_freeze_main(output: str) -> None:
    await corpus_freeze(output, disable_bars=None)


if __name__ == "__main__":
    with main():
        run(corpus_freeze_main(*argv[1:]))
