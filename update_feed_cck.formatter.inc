<?php
// $Id$

// Copyright (c) 2010 KontextWork
// Author: Eugen Mayer

/**
 * Implementation of CCK's hook_field_formatter_info().
 */
function update_feed_cck_field_formatter_info() {
  return array(
    'default' => array(
      'label' => t('Table(all fields)'),
      'field types' => array('update_feed_cck'),
      'multiple values' => CONTENT_HANDLE_MODULE,
      'description' => t('Displays modules in a table with the shortname, server, api and version'),
    ),
    'api_version' => array(
      'label' => t('API Versions'),
      'field types' => array('update_feed_cck'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'description' => t('Displays the api-versions of the module attached'),
    ),
    'server_key' => array(
      'label' => t('Server key'),
      'field types' => array('update_feed_cck'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'description' => t('Displays the server_key of the  module attached'),
    ),
    'short_name' => array(
      'label' => t('Short Name(Module name)'),
      'field types' => array('update_feed_cck'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'description' => t('Displays the short_name of the module attached'),
    ),
    'module_release' => array(
      'label' => t('Release'),
      'field types' => array('update_feed_cck'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'description' => t('Displays the version of the module attached'),
    ),
  );
}

function theme_update_feed_cck_value($val, $name, $field) {
  return $val;
}

function update_feed_cck_table($element) {
  $childkeys = element_children($element);
  foreach($childkeys as $key) {
    $children[$key] = $element[$key]['#item'];
  }
  // lets sort the children by the shortname
  if(!is_array($children)) {
    $children = array();
  }

  usort($children,'_update_feed_cck_sort_by_name');
  $count = count($children);
  $fieldset = array( 
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => "Packages ($count)",
    '#value' => theme('update_feed_cck_package_table', $children, $element['#field_name'], $element['#node']->nid),
  );
    
  return theme('fieldset',$fieldset);
}

function _update_feed_cck_sort_by_name($a, $b) {  
  return strcasecmp($a['short_name'],$b['short_name']);  
}

/**
 * Theme function for the 'default' filefield formatter.
 */
function theme_update_feed_cck_formatter_default($element) {
  $module = $element['#item'];
  $field = content_fields($element['#field_name']);
  $output = theme('update_feed_cck_item', $module, $field);
  return $output;
}

function theme_update_feed_cck_formatter_server_key($element) {
  // Inside a View this function may be called with null data. In that case,
  // just return.
  if (empty($element['#item'])) { 
    return '';
  }
  return theme('update_feed_cck_value',$element['#item']['server_key'], 'server_key',  content_fields($element['#field_name']));
}

function theme_update_feed_cck_formatter_api_version($element) {
  // Inside a View this function may be called with null data. In that case,
  // just return.
  if (empty($element['#item'])) { 
    return '';
  }
  return theme('update_feed_cck_value',$element['#item']['api_version'], 'api_version',  content_fields($element['#field_name']));
}

function theme_update_feed_cck_formatter_module_release($element) {
  // Inside a View this function may be called with null data. In that case,
  // just return.
  if (empty($element['#item'])) { 
    return '';
  }
  return theme('update_feed_cck_value',$element['#item']['module_release'], 'module_release',  content_fields($element['#field_name']));
}

function theme_update_feed_cck_formatter_short_name($element) {
  // Inside a View this function may be called with null data. In that case,
  // just return.
  if (empty($element['#item'])) { 
    return '';
  }
  return theme('update_feed_cck_value',$element['#item']['short_name'], 'short_name',  content_fields($element['#field_name']));
}



