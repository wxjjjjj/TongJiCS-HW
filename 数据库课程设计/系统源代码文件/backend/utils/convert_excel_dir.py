import os
import sys
import argparse
import json

try:
    import pandas as pd
except Exception:
    pd = None

# 目标字段
TARGET_FIELDS = ['ship_id', 'datetime', 'lat', 'long', 'sea_temp', 'wave_height', 'wave_period', 'surge_direction', 'surge_height']

# 可能的源字段名（包含英文和常见中文变体）
SOURCE_CANDIDATES = {
    'ship_id': ['SIGN_SHIP', 'ship_id', 'ship', '船舶', '船号', '编号'],
    'datetime': ['YEAR-MONTH-DAY', 'datetime', 'date', 'time', '时间', '日期', '观测时间'],
    'lat': ['LATITUDE', 'lat', '纬度'],
    'long': ['LONGITUDE', 'long', 'lon', '经度'],
    'sea_temp': ['SEA_TEMP', 'sea_temp', 'sea temperature', '海温', '海水温度'],
    'wave_height': ['WAVE_HEIGHT', 'wave_height', 'wave height', '波高', '浪高'],
    'wave_period': ['WAVE_PERIOD', 'wave_period', 'wave period', '波周期', '周期'],
    'surge_direction': ['SURGE_DIRECTION', 'surge_direction', 'surge dir', '方向', '涌向'],
    'surge_height': ['SURGE_HEIGHT', 'surge_height', 'surge', '涌高', '涌浪高度'],
}

# 年月日时列候选
DATE_PART_CANDIDATES = {
    'year': ['YEAR', 'year', '年', '年份'],
    'month': ['MONTH', 'month', '月'],
    'day': ['DAY', 'day', '日'],
    'hour': ['HOUR', 'hour', '时', '小时'],
}


def find_best_column(df_cols, candidates):
    cols_lower = {c.lower(): c for c in df_cols}
    for cand in candidates:
        if cand.lower() in cols_lower:
            return cols_lower[cand.lower()]
    # 尝试部分匹配
    for cand in candidates:
        for col in df_cols:
            if cand.lower() in col.lower() or col.lower() in cand.lower():
                return col
    return None


def process_file(path, out_dir, date_input_fmt=None, preview=False):
    if pd is None:
        raise RuntimeError('pandas 未安装，请先运行: pip install pandas openpyxl xlrd')

    df = pd.read_excel(path)
    if preview:
        print(f'File: {path}')
        print('Columns:', list(df.columns))
        return

    mapping = {}
    for tgt in TARGET_FIELDS:
        src = find_best_column(df.columns, SOURCE_CANDIDATES.get(tgt, []))
        mapping[tgt] = src

    # ship_id 映射持久化文件
    map_file = os.path.join(out_dir, 'ship_id_map.json')
    if os.path.exists(map_file):
        try:
            with open(map_file, 'r', encoding='utf-8') as mf:
                ship_map = json.load(mf)
        except Exception:
            ship_map = {}
    else:
        ship_map = {}
    # 计算下一个 id
    try:
        existing_ids = [int(v) for v in ship_map.values()]
        next_id = max(existing_ids) + 1 if existing_ids else 1
    except Exception:
        next_id = 1

    # 查找是否存在单独的 YEAR/MONTH/DAY/HOUR 列
    date_parts_map = {}
    for part, cands in DATE_PART_CANDIDATES.items():
        date_parts_map[part] = find_best_column(df.columns, cands)

    out_rows = []
    for _, r in df.iterrows():
        out_row = {}
        # pre-get date parts columns
        y_col = date_parts_map.get('year')
        m_col = date_parts_map.get('month')
        d_col = date_parts_map.get('day')
        h_col = date_parts_map.get('hour')

        for tgt in TARGET_FIELDS:
            if tgt == 'datetime':
                src = mapping.get(tgt)
                # 如果直接映射到列，但该列恰好是 YEAR/MONTH/DAY/HOUR 其中之一，则优先合成完整 datetime
                parts_set = {c for c in (y_col, m_col, d_col) if c}
                if src and src in df.columns and src not in parts_set:
                    val = r[src]
                    out_row[tgt] = val
                else:
                    # 尝试使用 YEAR/MONTH/DAY[/HOUR] 合成
                    if y_col and m_col and d_col:
                        try:
                            y = r[y_col]
                            mo = r[m_col]
                            da = r[d_col]
                            hr = r[h_col] if (h_col and h_col in df.columns) else 0

                            def to_int(v):
                                if pd.isna(v):
                                    return 0
                                if isinstance(v, int):
                                    return int(v)
                                try:
                                    return int(float(v))
                                except Exception:
                                    # 尝试从字符串中提取数字
                                    s = str(v)
                                    digits = ''.join(ch for ch in s if ch.isdigit())
                                    return int(digits) if digits else 0

                            yi = to_int(y)
                            mi = to_int(mo)
                            di = to_int(da)
                            hi = to_int(hr)
                            val = f"{yi:04d}-{mi}-{di} {hi}"
                        except Exception:
                            val = ''
                    else:
                        val = ''

                    out_row[tgt] = val
            else:
                src = mapping.get(tgt)
                val = r[src] if (src and src in df.columns) else ''

                # 将 SURGE_DIRECTION 数值映射为 N/S/E/W
                if tgt == 'surge_direction':
                    out_val = ''
                    try:
                        if isinstance(val, str) and val.strip():
                            s = val.strip().upper()
                            if s and s[0] in ('N', 'S', 'E', 'W'):
                                out_val = s[0]
                            else:
                                deg = float(s)
                                deg = deg % 360
                                if (deg >= 315 or deg < 45):
                                    out_val = 'N'
                                elif deg >= 45 and deg < 135:
                                    out_val = 'E'
                                elif deg >= 135 and deg < 225:
                                    out_val = 'S'
                                else:
                                    out_val = 'W'
                        else:
                            if pd.isna(val):
                                out_val = ''
                            else:
                                deg = float(val)
                                if deg <= -90 or deg == -99.9:
                                    out_val = ''
                                else:
                                    deg = deg % 360
                                    if (deg >= 315 or deg < 45):
                                        out_val = 'N'
                                    elif deg >= 45 and deg < 135:
                                        out_val = 'E'
                                    elif deg >= 135 and deg < 225:
                                        out_val = 'S'
                                    else:
                                        out_val = 'W'
                    except Exception:
                        out_val = ''

                    out_row[tgt] = out_val
                elif tgt == 'ship_id':
                    # 将字符 ship_id 映射为整数（持久化到 ship_id_map.json）
                    orig = ''
                    try:
                        if pd.isna(val):
                            orig = ''
                        else:
                            orig = str(val).strip()
                    except Exception:
                        orig = str(val).strip() if val is not None else ''

                    if not orig:
                        out_row[tgt] = ''
                    else:
                        if orig in ship_map:
                            out_row[tgt] = ship_map[orig]
                        else:
                            ship_map[orig] = next_id
                            out_row[tgt] = next_id
                            next_id += 1
                else:
                    out_row[tgt] = val

        out_rows.append(out_row)

    import csv
    base = os.path.splitext(os.path.basename(path))[0]
    out_path = os.path.join(out_dir, base + '.csv')
    with open(out_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=TARGET_FIELDS)
        writer.writeheader()
        for row in out_rows:
            writer.writerow(row)

    print(f'Converted {path} -> {out_path}')

    # 写回 ship_map
    try:
        with open(map_file, 'w', encoding='utf-8') as mf:
            json.dump(ship_map, mf, ensure_ascii=False, indent=2)
    except Exception:
        pass


def main():
    parser = argparse.ArgumentParser(description='批量将 data 目录下的 Excel 文件转换为符合导入格式的 CSV')
    parser.add_argument('data_dir', help='包含 Excel 文件的目录')
    parser.add_argument('--out', '-o', help='输出目录（默认覆盖 data_dir 下的 converted）', default=None)
    parser.add_argument('--preview', action='store_true', help='仅预览每个文件的列名')
    parser.add_argument('--date-input-fmt', help='如果需要，指定输入日期格式，例如 "%Y/%m/%d %H:%M:%S"')
    args = parser.parse_args()

    data_dir = args.data_dir
    out_dir = args.out or os.path.join(data_dir, 'converted')
    os.makedirs(out_dir, exist_ok=True)

    files = [os.path.join(data_dir, f) for f in os.listdir(data_dir) if f.lower().endswith(('.xls', '.xlsx'))]
    if not files:
        print('未找到 Excel 文件。')
        return

    for f in files:
        try:
            process_file(f, out_dir, date_input_fmt=args.date_input_fmt, preview=args.preview)
        except Exception as e:
            print(f'处理 {f} 失败: {e}')


if __name__ == '__main__':
    main()
