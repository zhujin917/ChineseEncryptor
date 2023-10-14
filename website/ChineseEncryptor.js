class ChineseEncryptor {
    __VERSION__ = "v0.5.1（内部预览版本）";

    dic = [];
    dicDic = {};

    dec2bin(dec, maxLength) {
        return (dec >>> 0).toString(2).padStart(maxLength, "0");
    }

    bin2dec(bin) {
        return parseInt(bin, 2);
    }

    seededRandom(seed) {
        let random = ((seed * 9301 + 49297) % 233280) / 233280;
        return Number(random.toFixed(8));
    }

    constructor(keyString) {
        let range1 = 0xf7 - 0xb0;
        let range2 = 0xfe - 0xa1;
        let code1, code2;
        let ch = [];
        let chs;

        for (let i = 0; i < 4098; i += 1) {
            if (i == 1672) {
                code1 = 0xe3;
                code2 = 0xa7;
            }
            else if (i == 2879) {
                code1 = 0xe4;
                code2 = 0xa8;
            }
            else {
                code1 = i % range1 + 0xb0;
                code2 = i % range2 + 0xa1;
            }
            ch = [code1, code2];
            chs = new TextDecoder("gb2312").decode(new Uint8Array(ch));
            this.dic[i] = chs;
            this.dicDic[chs] = i;
        }

        if (keyString) {
            let randomNum = new Array(keyString.length).fill(null);
            for (let i = this.dic.length - 1; i >= 0; i -= 1) {
                let keyIndex = i % keyString.length;
                let seed = randomNum[keyIndex] ? Math.round(randomNum[keyIndex] * Math.pow(10, 8)) : keyString[keyIndex].charCodeAt(0);
                randomNum[keyIndex] = this.seededRandom(seed);

                let target = Math.floor(randomNum[keyIndex] * (i + 1));

                let tmp = this.dic[i];
                this.dic[i] = this.dic[target];
                this.dic[target] = tmp;

                this.dicDic[this.dic[i]] = i;
                this.dicDic[this.dic[target]] = target;
            }
        }
    }

    encode(sourceString) {
        let binStr = "";
        for (let decNum of new TextEncoder().encode(sourceString)) {
            binStr += this.dec2bin(decNum, 8);
        }

        let result = "";
        for (let i = 0; i < binStr.length; i += 12) {
            let twelveBin = binStr.slice(i, i + 12);
            let index = this.bin2dec(twelveBin.padEnd(12, "0"));
            result += this.dic[index];
            result += this.dic[4096].repeat(Math.floor((12 - twelveBin.length) / 8));
        }
        while ((result.length * 12) % 8 != 0) {
            result += this.dic[4097];
        }
        return result;
    }

    decode(encodedString) {
        let binStr = "";
        for (let chr of encodedString) {
            let index = this.dicDic[chr];
            if (index == 4096) {
                binStr = binStr.substring(0, binStr.length - 8);
                continue;
            }
            else if (index == 4097) {
                break;
            }
            binStr += this.dec2bin(index, 12);
        }

        let decArr = [];
        for (let i = 0; i < Math.floor(binStr.length / 8) * 8; i += 8) {
            decArr.push(this.bin2dec(binStr.slice(i, i + 8)));
        }
        return new TextDecoder().decode(new Uint8Array(decArr));
    }
}
