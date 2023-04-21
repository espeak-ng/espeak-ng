/*
 * Copyright (C) 2012-2013 Reece H. Dunn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.reecedunn.espeak;

import java.util.List;

import android.app.Activity;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class InformationListAdapter extends ArrayAdapter<Pair<String,String>>
{
    private final LayoutInflater inflater;

    static class ViewHolder
    {
        public TextView title;
        public TextView summary;
    }

    public InformationListAdapter(Activity context, List<Pair<String,String>> information)
    {
        super(context, R.layout.information_view, information);
        this.inflater = context.getLayoutInflater();
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent)
    {
        ViewHolder holder;
        if (convertView == null)
        {
            convertView = inflater.inflate(R.layout.information_view, parent, false);
            holder = new ViewHolder();
            holder.title = (TextView)convertView.findViewById(android.R.id.title);
            holder.summary = (TextView)convertView.findViewById(android.R.id.summary);
            convertView.setTag(holder);
        }
        else
        {
            holder = (ViewHolder)convertView.getTag();
        }

        Pair<String,String> info = getItem(position);
        holder.title.setText(info.first);
        holder.summary.setText(info.second);
        return convertView;
    }
}
