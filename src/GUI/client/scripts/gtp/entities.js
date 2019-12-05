'use strict';

const validTypes = ["number", "Boolean", "string", "Color", "Vertex", "Move", "List", "MultiLineList", "Alternative"];

class Color {
    constructor(color) {
        if (typeof color === "string") {
            color = color.toLowerCase();
            if (color === "w" || color === "white" || color === "b" || color === "black")
                this.color = color;
        }
        else
            throw "Parameter is not a color value";
    
    }

    toString() {
        return this.color;
    }
}

class Vertex {
    constructor(value) {
        if (typeof value === "string") {
            if (!(value === "pass")) {
                if (value.length > 3)
                    throw "invalid vertex value";

                if (value[0] === 'i' || value[0] === 'I')
                    throw "invalid vertex value";

                let row = value.slice(1, 3);
                if (!isNaN(row) && row > 25)
                    throw "Invalid vertex value: protocol doesn't support boards larger than 25x25";

                let columnCharCode = value.charCodeAt(0);
                let column = 26;
                if (columnCharCode >= 65 && columnCharCode <= 90)
                    column = (columnCharCode[0] < 73) ? columnCharCode - 64 : columnCharCode - 65;
                else if (columnCharCode >= 97 && columnCharCode <= 122)
                    column = (columnCharCode < 105) ? columnCharCode - 96 : columnCharCode - 97;

                if (column > 25)
                    throw "Invalid vertex value: protocol doesn't support boards larger than 25x25";
            }
            this.vertex = value;
        }
        else
            throw "Invalid Vertex Value";
    }

    toString() {
        return this.vertex;
    }
};

class Move {
    constructor(move) {
        if (typeof move === "string") {
            move = move.split(' ');
            if (move.length !== 2)
                throw "Invalid Move must be string containing a Color and a Vertex separated by a space";
    
            let color = null;
            let vertex = null;
            try {
                color = new Color(move[0]);
                vertex = new Vertex(move[1]);
            }
            catch (exception) {
                throw "Invalid Move must be string containing a Color and a Vertex separated by a space";
            }

            this.move = `${color.toString()} ${vertex.toString()}`;
        }
        else
            throw "Invalid Move must be string containing a Color and a Vertex separated by a space";
    }

    toString() {
        return this.move;
    }
};

class List {
    constructor(type) {
        if (!validTypes.includes(type))
            throw "Invalid List type";
        
        this.type = type;
        this.items = [];
    }

    append(item) {
        if (typeof item !== this.type)
            throw "Invalid List Type";

        this.items.push(item);
    }

    appendAll(items) {
        for (index in items) {
            if (typeof items[index] !== this.type)
                throw "Invalid List Type";

            this.items.push(items[index]);
        }
    }

    toString() {
        let result = "";
        for (let i = 0; i < this.items.length; i++) {
            if (result === "")
                result = this.items[i].toString();
            else
                result += " " + this.items[i].toString();
        }

        return result;
    }

    apply(func) {
        for (let index in this.items) {
            this.items[index] = func(this.items[index]);
        }
    }
};

class MultiLineList {
    constructor(type) {
        if (!validTypes.includes(type))
            throw "Invalid MultiLineList type";
        
        this.type = type;
        this.items = [];

    }

    append(item) {
        if (typeof item !== this.type)
            throw "Invalid List Type";

        this.items.push(item);
    }

    appendAll(items) {
        for (index in items) {
            if (typeof items[index] !== this.type)
                throw "Invalid List Type";

            this.items.push(items[index]);
        }
    }

    toString() {
        let result = "";
        for (let i = 0; i < this.items.length; i++)
            result += this.items[i].toString() + "\n";

        return result;
    }

    apply(func) {
        for (let index in this.items) {
            this.items[index] = func(this.items[index]);
        }
    }
};
