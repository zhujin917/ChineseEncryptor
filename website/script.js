let encryptor = new ChineseEncryptor();
function initEncryptor() {
    if (contentDom.value.length == 0) {
        ui.alert("错误", "文本为空。");
        return;
    }
    encryptor = new ChineseEncryptor(document.getElementById("secretKey").value);
};

let contentDom;

window.addEventListener("load", () => {
    contentDom = document.getElementById("content");

    window.addEventListener("mousedown", () => {
        document.getElementById("menu").hidden = true;
    });
    document.getElementById("more").addEventListener("click", () => {
        document.getElementById("menu").hidden = false;
    });
    document.getElementById("menu").addEventListener("mousedown", (evt) => {
        evt.stopPropagation();
    });
    document.getElementById("menu").addEventListener("mouseup", (evt) => {
        document.getElementById("menu").hidden = true;
    });

    document.getElementById("menu_import").addEventListener("click", () => {
        ui.alert("敬请期待", "此功能将在不久后上线。");
    });
    document.getElementById("menu_export").addEventListener("click", () => {
        ui.alert("敬请期待", "此功能将在不久后上线。");
    });
    document.getElementById("menu_github").addEventListener("click", () => {
        window.open("https://github.com/zhujin917/ChineseEncryptor", "_blank");
    });
    document.getElementById("menu_website").addEventListener("click", () => {
        window.open("https://www.jiuxiaoqi.top/", "_blank");
    });
    document.getElementById("menu_about").addEventListener("click", () => {
        ui.alert("关于", `ChineseEncryptor ${encryptor.__VERSION__}`);
    });

    document.getElementById("encrypt").addEventListener("click", () => {
        initEncryptor();
        contentDom.value = encryptor.encode(contentDom.value);
    });
    document.getElementById("decrypt").addEventListener("click", () => {
        initEncryptor();
        contentDom.value = encryptor.decode(contentDom.value);
    });
});